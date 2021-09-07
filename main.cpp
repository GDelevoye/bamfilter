#include <boost/filesystem.hpp> // Files and paths
#include <boost/format.hpp> // Format strings

//Last versions are not shipped on conan center, and it is a header-only library, so I just included them here

#include "include/filtering.hpp"
#include <lyra/lyra.hpp> // argument parser

#include <string>
#include <map>

#include "spdlog/spdlog.h" // Logger
#include "spdlog/sinks/basic_file_sink.h"
#include <thread> // CPU Count

#include <iostream>


template <typename T>
bool fraction_validator(T fraction, std::shared_ptr<spdlog::logger> logger){
    if (fraction <= 1 && fraction >= 0){
        return true;
    }
    else{
        logger->warn((boost::format("The following fraction %1% is not within [0;1]") % fraction).str());
        return false;
    }
}

unsigned int handle_CPU_threads(unsigned int nbthread_asked,std::shared_ptr<spdlog::logger> logger){
    // Get the count of CPUlogger
    //may return 0 when not able to detect --> https://stackoverflow.com/questions/150355/programmatically-find-the-number-of-cores-on-a-machine
    unsigned int processor_count = std::thread::hardware_concurrency();
    unsigned int really_used = 0;

    if (processor_count)    {
        boost::format fmt_string = boost::format("Detected %1% CPU") % processor_count;
        logger->info(fmt_string.str());
    }
    else {
        logger->warn("Could not determine the number of available CPU. Will use only 1");
        really_used = 1;
    }

    if (nbthread_asked > processor_count){
        logger->warn((boost::format("Asked %1% threads but only %2% are available. %2% threads will be used.") % nbthread_asked % processor_count).str());
        really_used = processor_count;

    }
    else if (nbthread_asked == 0){
        logger->warn("Asked 0 threads but at least one is needed. Perhaps you meant 10 ?");
        logger->warn("nb_proc is set to 1 thread");
        really_used = 1;
    }
    else{
        really_used = nbthread_asked;
    }

    logger->info((boost::format("%1% CPU will be used.") % really_used).str());
    return really_used;
}

int main(int argc, char* argv[]) {
    // Where we read in the argument values:

    bool show_help = false;
    std::string rejected ="./rejected.bam";
    std::string outputlog = "./log_bamfile.txt";
    std::string verbose_level = "DEBUG";
    unsigned int nb_proc_asked= 1;
    float min_identity = 1;

    // The parser with the multiple option arguments. They are composed
    // together by the "|" operator.
    auto cli = lyra::cli();

    cli.add_argument(lyra::help(show_help));

    cli.add_argument(lyra::opt(rejected, "rejected_reads")
                             .optional()
                             .help("How wide should it be? <DEFAULT : ./rejected.bam>"));

    cli.add_argument(lyra::opt(outputlog, "output_log")
                             .optional()
                             .help("Path to a logfile <DEFAULT : ./log_bamfile.txt>"));

    cli.add_argument(lyra::opt(verbose_level, "verbosity")
                             .name("-v").name("--verbosity")
                             .optional()
                             .choices("DEBUG", "INFO", "WARN", "ERROR", "CRITICAL")
                             .help("Verbosity. <DEFAULT : DEBUG>"));

    cli.add_argument(lyra::opt(nb_proc_asked, "nb_proc")
                             .name("-n").name("--nb_proc")
                             .optional()
                             .help("Max number of threads <DISABLED> <DEFAULT : 1>"));

    cli.add_argument(lyra::opt(min_identity, "min_identity")
                             .name("-i").name("--min_identity")
                             .optional()
                             .help("Max number of threads <DISABLED> <DEFAULT : 1>"));


    // Parse the program arguments:
    auto result = cli.parse({argc, argv});

    // Check that the arguments where valid:
    if (!result) {
        std::cerr << "Error in command line: " << result.errorMessage() << std::endl;
        std::cerr << "Use bamfilter -h to display help." << std::endl;
        return 1;
    }


    // Show the help when asked for.
    if (show_help) {
        std::string description = "\n"
                                  "********************************************\n"
                                  "A lightweight C++ tool to filter .bam files.\n"
                                  "********************************************\n"
                                  "\n"
                                  "At the moment, only 1 criteria can be specified to allow a read to pass or not :\n"
                                  "- Its identity score : 2 * CIGARmatches / (seqlen + reflen)\n"
                                  "\n"
                                  "/!\\ This identity score is considerably different than the usual BLAST scores /!\\ \n"
                                  "\n"
                                  "It is meant to be used alongside with samtools view\n"
                                  "For instance: "
                                  "\n"
                                  "samtools view -h ./input.bam | bamfilter | samtools view -bS > output.bam\n"
                                  "\n";

        std::cout << description << std::endl;
        std::cout << cli << "\n";
        return 0;
    }

    // Handling logger
    auto path_to_log = boost::filesystem::absolute(outputlog);
    try {
        auto logger = spdlog::basic_logger_mt("bam_logger", path_to_log.string());
    }
    catch (const spdlog::spdlog_ex &ex) {
        std::cerr << "Log init failed: " << ex.what() << std::endl;
    }

    auto logger = spdlog::get("bam_logger");
    logger->set_pattern("[%H:%M:%S %z] [%^(---%l---)%$] [thread %t] %v");
    logger->flush_on(spdlog::level::debug);

    // Handling logger's verbosity
    bool failed = false;
    if (verbose_level == "DEBUG") {
        logger->set_level(spdlog::level::debug);
        logger->debug("Verbosity set on debug.");
    } else if (verbose_level == "INFO") {
        logger->set_level(spdlog::level::info);
    } else if (verbose_level == "WARN") {
        logger->set_level(spdlog::level::warn);
    } else if (verbose_level == "CRITICAL") {
        logger->set_level(spdlog::level::critical);
    } else {
        logger->set_level(spdlog::level::debug);
        {
            failed = true;
        }
    }

    if (failed) {
        boost::format fmt_string = boost::format("Could not recognize verboselevel %1%") % verbose_level;
        std::string astring = fmt_string.str();
        logger->warn(astring);
        logger->info("Will consider that verbosity is set to: [ DEBUG ]");
    }

    logger->info((boost::format("Welcome in %1%") % argv[0]).str());

    logger->debug((boost::format("min_percentage is set to %1%") % min_identity).str());

    if (not fraction_validator(min_identity, logger)) {
        logger->critical((boost::format("min_percentage is set to %1% although it should be within [0,1]") %
                          min_identity).str());
        throw std::invalid_argument("Invalid argument.");
    }

    // Handle the number of threads
    unsigned int nb_proc = handle_CPU_threads(nb_proc_asked, logger);
    logger->debug((boost::format("%1% CPU will be used.") % nb_proc).str());

    unsigned long nbkept = 0;
    unsigned long nbrejected = 0;

    // Handling the path to rejected reads
    auto path_to_rejected = boost::filesystem::absolute(rejected);

    auto filout = std::ofstream(rejected);

    for (std::string line; std::getline(std::cin, line);) {
//            logger->debug((boost::format("Line = %1%") % line).str());
        if (line[0] == '@') {
            std::cout << line << std::endl;
        } else if (filtering::isokay_filtering(min_identity, line)) {
            std::cout << line << std::endl;
            nbkept++;
        } else {
            filout << line << std::endl;
            nbrejected++;
        }
    }

    // Close the file
    filout.close();

    logger->info((boost::format("DONE. Accepted %1% reads and rejected %2%") % nbkept % nbrejected).str());

    return 0;
}