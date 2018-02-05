import qbs

Project {
    CppApplication {
        consoleApplication: true
        Group {
            fileTagsFilter: "application"
            qbs.install: true
        }

        cpp.cppFlags: "-std=c++14"
        cpp.includePaths: product.sourceDirectory

        Depends { name: "Qt"; submodules: ["core", "network", ] }

        Group {
            name: 'src'
            files: ["*.c", "*.h", "*.cpp"]
            prefix: "**/"
        }
    }
}
