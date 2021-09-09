# bam_filter

**Description**

Lightweight and fast C++ CLI tool to filter a .bam file alonside with samtools view 


# Dev status

Prototype

> :warning: Multi-threading is not implemented, and @SQlines of the header are all kept in the output.


> :warning: No test is implemented yet (prototype)

# Platforms/OS

Only tested in Ubuntu LTS 20.04 x86_64

As it is pure C++/CMake project, it should be easily compiled on many other platforms, including MacOS and Windows.

# Usage

```console
********************************************
A lightweight C++ tool to filter .bam files.
********************************************
	

It is meant to be used alongside with samtools view
For instance: 

samtools view -h ./input.bam | bamfilter | samtools view -bS > output.bam

At the moment, only 1 criteria can be specified to allow a read to pass or not :
- Its identity score : 2 * CIGARmatches / (seqlen + reflen)

/!\ This identity score is considerably different than the usual BLAST scores /!\ 

USAGE:
  bamfilter [-?|-h|--help] [ <rejected_reads>] [ <output_log>] [-v|--verbosity <verbosity>] [-n|--nb_proc <nb_proc>] [-i|--min_identity <min_identity>]

Display usage information.

OPTIONS, ARGUMENTS:
  -?, -h, --help          
   <rejected_reads>       How wide should it be? <DEFAULT : ./rejected.bam>
   <output_log>           Path to a logfile <DEFAULT : ./log_bamfile.txt>
  -v, --verbosity <verbosity>
                          Verbosity. <DEFAULT : DEBUG>
  -n, --nb_proc <nb_proc> Max number of threads <DISABLED> <DEFAULT : 1>
  -i, --min_identity <min_identity>
                          Max number of threads <DISABLED> <DEFAULT : 1>

```

# Build from sources

No pre-compiled binary executable is distributed for now.
You have to compile it from sources.

## Requirements

*Optional :* 

- [Conda](https://anaconda.org/conda-forge/conan)
> You don't need it if conan and git are already installed.)

**Mandatory :**

- A [C++11 compliant](https://en.wikipedia.org/wiki/C17_(C_standard_revision)) compiler
- [CMake](https://cmake.org/download/)
 

##  Build instruction

*Optional (You can skip if conan and git are already installed)*

1. *Download the environment.yml file*
2. *Create a virtual environment from it, and activate the environment :*

```console
(base) user@computer:$ conda env create -n myenv -f ./environment.yml
(base) user@computer:$ conda activate -n myenv
(myenv) user@computer:$
```

**Mandatory :**

3. Then, just copy past the following script, that will clone the current repository, install all dependencies and build the executable

```console
git clone https://github.com/GDelevoye/bam_filter/ &&
cd bam_filter &&
git pull &&
mkdir -p build && cd build &&
conan install .. --build=missing &&
cmake .. -G "Unix Makefiles" &&
make &&
echo "DONE"
```

Once you built the program, the executable will be present in **./bam_filter/bin/bamfilter**

> Manually copy this executable to a directory that is located in your $PATH (in UNIX systems, run ```echo $PATH``` to know which are available). 

## Known troubles

In recent Linux systems, the building should be done without problems.

On older distributions however, some issues can occur regarding the C++11 standard.

In case of troubles :

- [Check your ABI](https://docs.conan.io/en/latest/howtos/manage_gcc_abi.html). It must be C++11 compatible
- In ~/.conan/conan.conf you can also :
	* Disable the lock in the cache (by editing ~/.conan/conan.conf) (uncomment ```cache_no_locks = True```)
	* Overright the path to cmake or compiler, to ones that are compliant with C++11
- Set your compiler version in your conan profile : ```conan profile update 'settings.compiler.version=yourversion' default```
- Clear the cache : ```conan remove '*' -f ```
- If your system has several compiler installed, perhaps the default is not C++11 compliant. Export the right environment vaiable to point to the right one could fix the problem too. Exemple

```console
> export CC="gcc-6"
> export CXX="g++-6"
```

# LICENCE

LGPL v3.0

# Credits

DELEVOYE Guillaume
delevoye.guillaume@gmail.com
delevoye@ens.fr


