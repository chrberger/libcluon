How to contribute to the development of libcluon
================================================

Thank you for taking a moment to contribute to read these continuously evolving instructions how to contribute to libcluon.
To maintain a high quality of our code base, we are very strict in terms of reviewing and accepting pull requests (PR).

## Getting Started
- Make sure you have a GitHub account.
- Make sure you use an up-to-date C++14 compiler; we are usually developing with GNU GCC.
- Register a report about your issue but check beforehand [our issue list](https://github.com/chrberger/libcluon/issues) for similar problems.

## Making Changes
- Create a named branch where you want to base your work.
    - We are using the YYYYQN.Z.topic scheme with small letters, where YYYY is the year, QN is the current quarter, Z is one of `feature` or `fix`, and Z is the topic. Example: 2017Q4.feature.awesome-feature. This scheme allows us to quickly list and sort branches.
    - Make sure that your topic branched off from master.
- Make changes and commits.
- We are only reviewing PRs when you have added sufficient test cases and maintain a good test coverage. We are using [![CodeCov](https://codecov.io/gh/chrberger/libcluon)](https://codecov.io/gh/chrberger/libcluon) for visualizing code coverage.
- Make sure you're sticking with our code style. You can run [`clang-format`](http://clang.llvm.org/docs/ClangFormat.html) manually or using our Docker development image. 
- We highly encourage the use of our Docker development image:
    - Creating the Docker development image: `make createDockerBuildImage`
    - Cleaning the build folder: `make clean`
    - Running a build: `make compile && make test`
    - Re-formatting the code according to our coding guidelines: `make reformat-code`
    - Running our static-code-analysis (currently including: [`scan-build`](https://clang-analyzer.llvm.org/scan-build.html), [`clang-tidy`](http://clang.llvm.org/extra/clang-tidy/), [`flawfinder`](https://www.dwheeler.com/flawfinder/), [`oclint`](https://github.com/oclint/oclint), [`vera++`](https://bitbucket.org/verateam/vera/wiki/Home), [`cppcheck`](http://cppcheck.sourceforge.net/), and [`Flint++`](https://github.com/L2Program/FlintPlusPlus)): `make static-code-analysis`
    - Any PRs that are not following these recommendations will not be considered for potential inclusion!

## Coding Guidelines
- Keep the highest possible warning level.
- We treat warnings as errors.
- Keep C++14-only.
- Keep dependency-free.
- Keep platform-independent (i.e., prefer C++ standard library instead of Windows/POSIX dependent API).
