//===----------------------------------------------------------------------===//
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <iostream>
#include <cassert>
#include "clamp-config.hxx"
/* Flag set by ‘--verbose’. */
static int verbose_flag;
static bool build_mode = false, install_mode = true; // use install mode by default
static bool opencl_mode = true, hsa_mode = false; // use opencl mode by default

void replace(std::string& str,
        const std::string& from, const std::string& to) {
    size_t start_pos = str.find(from);
    while(start_pos != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos = str.find(from);
    }
}

void cxxflags(void) {
    if (!build_mode && !install_mode) {
        std::cerr << "Please specify --install or --build mode before flags\n";
        abort();
    }

    // Common options
    std::cout << "-std=c++amp";

    //if (hsa_mode) {
      // HSA header
      //std::cout << " -I" CMAKE_HSA_ROOT;
    //}

    //if (opencl_mode) {
      // OpenCL header
      //std::cout << " -I" CMAKE_OPENCL_INC;
      //char *NV = getenv("CXXAMP_NV");
      //if (NV != nullptr)
      //  std::cout << " -DCXXAMP_NV=1 ";
    //}

    // clamp
    if (build_mode) {
        std::cout << " -I" CMAKE_CLAMP_INC_DIR;
        // libcxx
        std::cout << " -I" CMAKE_LIBCXX_INC;
    } else if (install_mode) {
        std::cout << " -I" CMAKE_INSTALL_INC;
        std::cout << " -I" CMAKE_INSTALL_LIBCXX_INC;
    } else {
        assert(0 && "Unreacheable!");
    }

    std::cout << std::endl;
}

void ldflags(void) {
    // Common options
    std::cout << "-std=c++amp";
    if (build_mode) {
        std::cout << " -L" CMAKE_AMPCL_LIB_DIR;
        std::cout << " -L" CMAKE_LIBCXX_LIB_DIR;
        std::cout << " -L" CMAKE_LIBCXXRT_LIB_DIR;
        std::cout << " -Wl,--rpath="
            CMAKE_AMPCL_LIB_DIR ":"
            CMAKE_LIBCXX_LIB_DIR ":"
            CMAKE_LIBCXXRT_LIB_DIR ;
    } else if (install_mode) {
        std::cout << " -L" CMAKE_INSTALL_LIB;
        std::cout << " -Wl,--rpath=" CMAKE_INSTALL_LIB;
    }
    if (hsa_mode) {
      // HSA libraries
      //std::cout << " -Wl,--rpath=" CMAKE_HSA_LIB;
      //std::cout << " -L" CMAKE_HSA_LIB;
      std::cout << " -Wl,--whole-archive -lmcwamp_hsa -Wl,--no-whole-archive ";
      //std::cout << " -lelf -lhsa-runtime64 ";
      //std::cout << " " CMAKE_LIBHSAIL_LIB;
      //std::cout << " -Wl,--unresolved-symbols=ignore-in-shared-libs ";
      //std::cout << " -lpthread ";
    }

    if (opencl_mode) {
      // OpenCL library
      //std::cout << " -L" CMAKE_OPENCL_LIB;
      std::cout << " -Wl,--whole-archive -lmcwamp_opencl -Wl,--no-whole-archive ";
      //std::cout << " -lOpenCL";
    }

    std::cout << " -lc++ -lcxxrt -ldl ";
    std::cout << "-Wl,--whole-archive -lmcwamp -Wl,--no-whole-archive ";
}

void prefix(void) {
    std::cout << CMAKE_INSTALL_PREFIX;
}

int main (int argc, char **argv) {
    int c;
    while (1)
    {
        static struct option long_options[] =
        {
            /* These options set a flag. */
            {"verbose", no_argument,       &verbose_flag, 1},
            {"brief",   no_argument,       &verbose_flag, 0},
            /* These options don't set a flag.
               We distinguish them by their indices. */
            {"cxxflags", no_argument,       0, 'a'},
            {"build",    no_argument,       0, 'b'},
            {"install",  no_argument,       0, 'i'},
            {"ldflags",  no_argument,       0, 'l'},
            {"prefix",  no_argument,       0, 'p'},
            {"opencl",  no_argument,       0, 'o'},
            {"hsa",     no_argument,       0, 'h'},
            {0, 0, 0, 0}
        };
        /* getopt_long stores the option index here. */
        int option_index = 0;

        c = getopt_long (argc, argv, "",
                long_options, &option_index);

        /* Detect the end of the options. */
        if (c == -1)
            break;

        switch (c)
        {
            case 0:
                /* If this option set a flag, do nothing else now. */
                if (long_options[option_index].flag != 0)
                    break;
                printf ("option %s", long_options[option_index].name);
                if (optarg)
                    printf (" with arg %s", optarg);
                printf ("\n");
                break;

            case 'a':   // --cxxflags
                cxxflags();
                break;
            case 'l':   // --ldflags
                ldflags();
                break;
            case 'p':   // --prefix
                prefix();
                break;
            case 'b':   // --build
                build_mode = true;
                install_mode = false;
                break;
            case 'i':   // --install
                build_mode = false;
                install_mode = true;
                break;

            case 'o':   // --opencl
                opencl_mode = true;
                hsa_mode = false;
                break;
            case 'h':   // --hsa
                opencl_mode = false;
                hsa_mode = true;
                break;

            case '?':
                /* getopt_long already printed an error message. */
                break;

            default:
                abort ();
        }
    }

    /* Instead of reporting ‘--verbose’
       and ‘--brief’ as they are encountered,
       we report the final status resulting from them. */
    if (verbose_flag)
        puts ("verbose flag is set");

    /* Print any remaining command line arguments (not options). */
    if (optind < argc)
    {
        printf ("non-option ARGV-elements: ");
        while (optind < argc)
            printf ("%s ", argv[optind++]);
        putchar ('\n');
    }

    exit (0);
}
