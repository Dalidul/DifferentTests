import qbs

Project {
    CppApplication {
        consoleApplication: true
        Group {
            fileTagsFilter: "application"
            qbs.install: true
        }

        Depends { name: "Qt"; submodules: ["core", ] }

        cpp.cppFlags: "-std=c++11"

        Group {
            name: 'src'
            files: ["*.c", "*.h", "*.cpp"]
            prefix: "**/"
        }
    }
}
