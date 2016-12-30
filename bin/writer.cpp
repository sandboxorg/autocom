//  :copyright: (c) 2015-2016 The Regents of the University of California.
//  :license: MIT, see licenses/mit.md for more details.
/*
 *  \addtogroup AutoCom
 *  \brief Write class attributes into C++ interface.
 */

#include "writer.hpp"

#include <fstream>
#include <ostream>
#include <sstream>              // TODO: remove
#include <iostream>             // TODO: REMOVE


namespace autocom
{
// FUNCTIONS
// ---------


/** \brief Write documentation string for library to file.
 */
void writeDocString(std::ostream &stream)
{
    stream << "/**\r\n"
           << " *            **DO NOT EDIT THIS FILE**              \r\n"
           << " *  This file was automatically generated by AutoCom.\r\n"
           << " *  Any changes to this file will be overwritten.    \r\n"
           << " */\r\n"
           << "\r\n";
}


/** \brief Write import statement for file.
 */
void writeImportStatement(std::ostream &stream,
    TypeLibDescription &tlib)
{
    stream << "#include \"" << tlib.guid.string() << ".hpp\"\r\n";
}


/** \brief Write section for C++
 */
template <
    typename Container,
    typename Stringify
>
void writeSection(std::ostream &stream,
    const Container &container,
    const std::string &comment,
    Stringify stringify)
{
    stream << "// " << comment << "\r\n"
           << "// " << std::string(comment.size(), '-') << "\r\n"
           << "\r\n";

    for (const auto &item : container) {
        stream << stringify(item) << "\r\n";
    }
    stream << "\r\n";
}


/** \brief Write human-friendly import library.
 */
std::string writeImportHeader(TypeLibDescription &tlib,
    std::string &directory)
{
    // get path
    auto name = tlib.documentation.name + ".hpp";
    std::string path = directory + "\\" + name;
    std::ofstream stream(path, std::ios::binary);

    // write import
    writeDocString(stream);
    writeImportStatement(stream, tlib);

    return path;
}


/** \brief Write header with unique CLSID as an identifier.
 */
std::string writeClsidHeader(TypeLibDescription &tlib,
    std::string &directory)
{
    auto name = tlib.guid.string() + ".hpp";
    std::string path = directory + "\\" + name;
    std::ofstream stream(path, std::ios::binary);

    // write data
    writeDocString(stream);
    stream << "#include <autocom.hpp>\r\n\r\n";

    writeSection(stream, tlib.description.enums, "ENUMS", [](const detail::Enum &item) {
        return item.header();
    });
    writeSection(stream, tlib.description.records, "FORWARD", [](const detail::Record &item) {
        return item.forward();
    });
    writeSection(stream, tlib.description.records, "STRUCTS", [](const detail::Record &item) {
        return item.header();
    });
    writeSection(stream, tlib.description.interfaces, "INTERFACES", [](const detail::Interface &item) {
        return item.header();
    });

    return path;
}


/** \brief Write C++ header file from TypeLib description.
 */
void writeHeaders(TypeLibDescription &tlib,
    std::string &directory,
    Files &files)
{
    files.headers.emplace_back(writeImportHeader(tlib, directory));
    files.headers.emplace_back(writeClsidHeader(tlib, directory));
}

/** \brief Write C++ source file from TypeLib description.
 */
void writeSources(TypeLibDescription &tlib,
    std::string &directory,
    Files &files)
{
    auto name = tlib.guid.string() + ".cpp";
    std::string path = directory + "\\" + name;
    std::ofstream stream(path, std::ios::binary);
    files.sources.emplace_back(path);

    // write data
    writeDocString(stream);
    stream << "#include \"" << tlib.guid.string() << ".hpp\"\r\n\r\n";

    writeSection(stream, tlib.description.interfaces, "INTERFACES", [](const detail::Interface &item) {
        return item.source();
    });
}


}   /* autocom */
