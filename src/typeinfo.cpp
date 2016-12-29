//  :copyright: (c) 2015-2016 The Regents of the University of California.
//  :license: MIT, see licenses/mit.md for more details.
/*
 *  \addtogroup AutoCom
 *  \brief C++ handle around ITypeInfo.
 */

#include "autocom.hpp"


namespace autocom
{
// FUNCTIONS
// ---------


/** \brief Create new handle to ITypeInfo object from IDispatch.
 */
ITypeInfo * newTypeInfo(IDispatch *dispatch)
{
    ITypeInfo *ppv = nullptr;
    if (FAILED(dispatch->GetTypeInfo(0, LOCALE_USER_DEFAULT, &ppv))) {
        throw GetTypeInfoError();
    }
    return ppv;
}


/** \brief Create new handle to ITypeInfo object from ITypeLib.
 */
ITypeInfo * newTypeInfo(ITypeLib *tlib,
    const UINT index)
{
    ITypeInfo *ppv = nullptr;
    if (FAILED(tlib->GetTypeInfo(index, &ppv))) {
        throw GetTypeInfoError();
    }
    return ppv;
}


/** \brief Create new handle to ITypeLib object from ITypeInfo.
 */
ITypeLib * newTypeLib(ITypeInfo *info)
{
    ITypeLib *ppv = nullptr;
    UINT index;
    if (FAILED(info->GetContainingTypeLib(&ppv, &index))) {
        throw GetContainingTypeLibError();
    }

    return ppv;
}


/** \brief Create new handle to TYPEATTR object from ITypeInfo.
 */
TYPEATTR * newTypeAttr(ITypeInfo *info)
{
    TYPEATTR *attr;
    if (FAILED(info->GetTypeAttr(&attr))) {
        throw GetTypeAttrError();
    }

    return attr;
}


/** \brief Create new handle to VARDESC object from ITypeinfo.
 */
VARDESC * newVarDesc(ITypeInfo *info,
    const UINT index)
{
    VARDESC *desc;
    if (FAILED(info->GetVarDesc(index, &desc))) {
        throw GetVarDescError();
    }

    return desc;
}


/** \brief Create new handle to TLIBATTR object from ITypeLib.
 */
TLIBATTR * newTypeLibAttr(ITypeLib *tlib)
{
    TLIBATTR *attr;
    if (FAILED(tlib->GetLibAttr(&attr))) {
        throw GetLibAttrError();
    }

    return attr;
}


/** \brief Read documentation from interface.
 */
template <
    typename Type,
    typename Identifier
>
Documentation getDocumentation(Type *ppv,
    const Identifier id)
{
    Documentation documentation;
    Bstr name, doc, file;
    DWORD help;

    if (FAILED(ppv->GetDocumentation(id, &name.string, &doc.string, &help, &file.string))) {
        throw GetDocumentationError();
    }

    documentation.name = std::string(name);
    documentation.doc = std::string(doc);
    documentation.help = help;
    documentation.file = std::string(file);

    return documentation;
}


// OBJECTS
// -------


/** \brief Equality operator.
 */
bool operator==(const TypeInfo &left,
    const TypeInfo &right)
{
    return equalObject(left.ppv, right.ppv);
}


/** \brief Inequality operator.
 */
bool operator!=(const TypeInfo &left,
    const TypeInfo &right)
{
    return !operator==(left, right);
}


/** \brief Initialize class from pointer.
 */
TypeInfo::TypeInfo(ITypeInfo *info)
{
    open(info);
}


/** \brief Open handle to ITypeInfo model.
 */
void TypeInfo::open(ITypeInfo *info)
{
    if (info) {
        ppv.reset(info, destroy<ITypeInfo>);
    } else {
        ppv.reset();
    }
}


/** \brief Construct ITypeLib from pointer.
 */
TypeLib TypeInfo::typelib() const
{
    return TypeLib(newTypeLib(ppv.get()));
}


/** \brief Get attributes for TypeInfo.
 */
TypeAttr TypeInfo::attr() const
{
    return TypeAttr(ppv);
}


/** \brief Get documentation from TypeInfo.
 */
Documentation TypeInfo::documentation(const MEMBERID id) const
{
    return getDocumentation(ppv.get(), id);
}


/** \brief Equality operator.
 */
bool operator==(const TypeLib &left,
    const TypeLib &right)
{
    return equalObject(left.ppv, right.ppv);
}


/** \brief Inequality operator.
 */
bool operator!=(const TypeLib &left,
    const TypeLib &right)
{
    return !operator==(left, right);
}


/** \brief Initialize class from pointer.
 */
TypeLib::TypeLib(ITypeLib *tlib)
{
    open(tlib);
}


/** \brief Open handle to ITypeLib model.
 */
void TypeLib::open(ITypeLib *tlib)
{
    if (tlib) {
        ppv.reset(tlib, destroy<ITypeLib>);
    } else {
        ppv.reset();
    }
}


/** \brief Get attributes for TypeLib.
 */
TypeLibAttr TypeLib::attr() const
{
    return TypeLibAttr(ppv);
}


/** \brief Get documentation from TypeLib.
 */
Documentation TypeLib::documentation(const INT index) const
{
    return getDocumentation(ppv.get(), index);
}


/** \brief Get number of type-descriptions in library.
 */
UINT TypeLib::count() const
{
    return ppv->GetTypeInfoCount();
}


/** \brief Get type info for member at index.
 */
TypeInfo TypeLib::info(const UINT index) const
{
    return TypeInfo(newTypeInfo(ppv.get(), index));
}


/** \brief Construct TYPEATTR from ITypeInfo.
 */
TypeAttr::TypeAttr(const ITypeInfoPtr &info)
{
    open(info);
}


/** \brief Open TYPEATTR from ITypeInfo.
 */
void TypeAttr::open(const ITypeInfoPtr &info)
{
    if (info) {
        attr.reset(newTypeAttr(info.get()), [info](TYPEATTR *ptr) {
            info->ReleaseTypeAttr(ptr);
        });
        ppv = info;
    } else {
        attr.reset();
        ppv.reset();
    }
}


/** \brief Check if TypeAttr is valid.
 */
TypeAttr::operator bool() const
{
    return bool(attr);
}


/** \brief Get Guid from TYPEATTR.
 */
Guid TypeAttr::guid() const
{
    return Guid(attr->guid);
}


/** \brief Get LCID from TYPEATTR.
 */
LCID TypeAttr::lcid() const
{
    return attr->lcid;
}


/** \brief Get MEMBERID of constructor from TYPEATTR.
 */
MEMBERID TypeAttr::constructor() const
{
    return attr->memidConstructor;
}


/** \brief Get MEMBERID of destructor from TYPEATTR.
 */
MEMBERID TypeAttr::destructor() const
{
    return attr->memidDestructor;
}


/** \brief Get size of type instance from TYPEATTR.
 */
ULONG TypeAttr::size() const
{
    return attr->cbSizeInstance;
}


/** \brief Get type kind from TYPEATTR.
 */
TYPEKIND TypeAttr::kind() const
{
    return attr->typekind;
}


/** \brief Get member function count from TYPEATTR.
 */
WORD TypeAttr::functions() const
{
    return attr->cFuncs;
}


/** \brief Get member variable count from TYPEATTR.
 */
WORD TypeAttr::variables() const
{
    return attr->cVars;
}


/** \brief Get interface count from TYPEATTR.
 */
WORD TypeAttr::interfaces() const
{
    return attr->cImplTypes;
}


/** \brief Get size of virtual table from TYPEATTR.
 */
WORD TypeAttr::vtblSize() const
{
    return attr->cbSizeVft;
}


/** \brief Get byte alignment from TYPEATTR.
 */
WORD TypeAttr::alignment() const
{
    return attr->cbAlignment;
}


/** \brief Get type flags from TYPEATTR.
 */
WORD TypeAttr::flags() const
{
    return attr->wTypeFlags;
}


/** \brief Get major version number from TYPEATTR.
 */
WORD TypeAttr::major() const
{
    return attr->wMajorVerNum;
}


/** \brief Get minor version number from TYPEATTR.
 */
WORD TypeAttr::minor() const
{
    return attr->wMinorVerNum;
}


/** \brief Get description of alias if kind() is TKIND_ALIAS.
 */
TYPEDESC TypeAttr::alias() const
{
    return attr->tdescAlias;
}


/** \brief Get IDL attributes of type.
 */
IDLDESC TypeAttr::idl() const
{
    return attr->idldescType;
}


/** \brief Construct TLIBATTR from ITypeLib.
 */
TypeLibAttr::TypeLibAttr(const ITypeLibPtr &tlib)
{
    open(tlib);
}


/** \brief Open TLIBATTR from ITypeLib.
 */
void TypeLibAttr::open(const ITypeLibPtr &tlib)
{
    if (tlib) {
        attr.reset(newTypeLibAttr(tlib.get()), [tlib](TLIBATTR *ptr) {
            tlib->ReleaseTLibAttr(ptr);
        });
        ppv = tlib;
    } else {
        attr.reset();
        ppv.reset();
    }
}


/** \brief Check if TypeLibAttr is valid.
 */
TypeLibAttr::operator bool() const
{
    return bool(attr);
}


/** \brief Get Guid from TLIBATTR.
 */
Guid TypeLibAttr::guid() const
{
    return Guid(attr->guid);
}


/** \brief Get LCID from TLIBATTR.
 */
LCID TypeLibAttr::lcid() const
{
    return attr->lcid;
}


/** \brief Get SYSKIND from TLIBATTR.
 */
SYSKIND TypeLibAttr::syskind() const
{
    return attr->syskind;
}


/** \brief Get major version number from TLIBATTR.
 */
WORD TypeLibAttr::major() const
{
    return attr->wMajorVerNum;
}


/** \brief Get minor version number from TLIBATTR.
 */
WORD TypeLibAttr::minor() const
{
    return attr->wMinorVerNum;
}


/** \brief Get library flags from TLIBATTR.
 */
WORD TypeLibAttr::flags() const
{
    return attr->wLibFlags;
}


/** \brief Open VARDESC from ITypeInfo.
 */
VarDesc::VarDesc(const ITypeInfoPtr &info)
{
    open(info);
}

/** \brief Construct VARDESC from ITypeInfo.
 */
void VarDesc::open(const ITypeInfoPtr &info)
{
//    if (info) {
//        desc.reset(newVarDesc(info.get()), [info](VARDESC *ptr) {
//            info->ReleaseVarDesc(ptr);
//        });
//        ppv = info;
//    } else {
//        desc.reset();
//        ppv.reset();
//    }
}


/** \brief Check if VarDesc is valid.
 */
VarDesc::operator bool() const
{
    return bool(desct);
}


}   /* autocom */