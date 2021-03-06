//  :copyright: (c) 2015-2016 The Regents of the University of California.
//  :license: MIT, see LICENSE.md for more details.
/*
 *  \addtogroup AutoCOM
 *  \brief C++ handle around ITypeInfo.
 */

#include "autocom/bstr.hpp"
#include "autocom/com.hpp"
#include "autocom/typeinfo.hpp"

#include <cassert>


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
        throw ComMethodError("IDispatch", "GetTypeInfo(0, LOCALE_USER_DEFAULT, ...)");
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
        throw ComMethodError("ITypeLib", "GetTypeInfo(0, LOCALE_USER_DEFAULT, ...)");
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
        throw ComMethodError("ITypeInfo", "GetContainingTypeLib(...)");
    }

    return ppv;
}


/** \brief Create new handle to TYPEATTR object from ITypeInfo.
 */
TYPEATTR * newTypeAttr(ITypeInfo *info)
{
    TYPEATTR *attr = nullptr;
    if (FAILED(info->GetTypeAttr(&attr))) {
        throw ComMethodError("ITypeInfo", "GetTypeAttr(...)");
    }

    return attr;
}


/** \brief Create new handle to VARDESC object from ITypeinfo.
 */
VARDESC * newVarDesc(ITypeInfo *info,
    const UINT index)
{
    VARDESC *desc = nullptr;
    if (FAILED(info->GetVarDesc(index, &desc))) {
        throw ComMethodError("ITypeInfo", "GetVarDesc(...)");
    }

    return desc;
}


/** \brief Create new handle to FUNCDESC object from ITypeinfo.
 */
FUNCDESC * newFuncDesc(ITypeInfo *info,
    const UINT index)
{
    FUNCDESC *desc = nullptr;
    if (FAILED(info->GetFuncDesc(index, &desc))) {
        throw ComMethodError("ITypeInfo", "GetFuncDesc(...)");
    }

    return desc;
}


/** \brief Create new handle to TLIBATTR object from ITypeLib.
 */
TLIBATTR * newTypeLibAttr(ITypeLib *tlib)
{
    TLIBATTR *attr = nullptr;
    if (FAILED(tlib->GetLibAttr(&attr))) {
        throw ComMethodError("ITypeLib", "GetLibAttr(...)");
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

    if (FAILED(ppv->GetDocumentation(id, &name.data(), &doc.data(), &help, &file.data()))) {
        throw ComMethodError("ITypeLib/ITypeInfo", "GetDocumentation(...)");
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
    ppv.reset(info);
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


/** \brief Get variable description at index.
 */
VarDesc TypeInfo::vardesc(const UINT index) const
{
    return VarDesc(ppv, index);
}


/** \brief Get function description at index.
 */
FuncDesc TypeInfo::funcdesc(const UINT index) const
{
    return FuncDesc(ppv, index);
}


/** \brief Get type info from reference to other type.
 */
TypeInfo TypeInfo::info(const HREFTYPE type) const
{
    ITypeInfo *info = nullptr;
    if (FAILED(ppv->GetRefTypeInfo(type, &info))) {
        throw ComMethodError("ITypeInfo", "GetRefTypeInfo()");
    }

    return TypeInfo(info);
}


/** \brief Get reference for any inherited intefaces.
 */
HREFTYPE TypeInfo::reference(const UINT index) const
{
    HREFTYPE ref;
    if (FAILED(ppv->GetRefTypeOfImplType(index, &ref))) {
        throw ComMethodError("ITypeInfo", "GetRefTypeOfImplType()");
    }

    return ref;
}


/** \brief Get reference for implied type.
 */
INT TypeInfo::flags(const UINT index) const
{
    INT flag;
    if (FAILED(ppv->GetImplTypeFlags(index, &flag))) {
        throw ComMethodError("ITypeInfo", "GetImplTypeFlags()");
    }

    return flag;
}


/** \brief Get function entry point.
 */
DllEntry TypeInfo::entry(const MEMBERID id,
    const INVOKEKIND invocation) const
{
    DllEntry entry;
    Bstr dll, name;
    WORD ordinal;
    if (FAILED(ppv->GetDllEntry(id, invocation, &dll.data(), &name.data(), &ordinal))) {
        throw ComMethodError("ITypeInfo", "GetDllEntry()");
    }

    return entry;
}


/** \brief Get documentation from TypeInfo.
 */
Documentation TypeInfo::GetDocumentation(const MEMBERID id) const
{
    return documentation(id);
}


/** \brief Get type info from reference to other type.
 */
TypeInfo TypeInfo::GetRefTypeInfo(const HREFTYPE type) const
{
    return info(type);
}


/** \brief Get reference for any inherited intefaces.
 */
HREFTYPE TypeInfo::GetRefTypeOfImplType(const UINT index) const
{
    return reference(index);
}


/** \brief Get reference for implied type.
 */
INT TypeInfo::GetImplTypeFlags(const UINT index) const
{
    return flags(index);
}


/** \brief Get function entry point.
 */
DllEntry TypeInfo::GetDllEntry(const MEMBERID id,
    const INVOKEKIND invocation) const
{
    return entry(id, invocation);
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
    ppv.reset(tlib);
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


/** \brief Get documentation from TypeLib.
 */
Documentation TypeLib::GetDocumentation(const INT index) const
{
    return documentation(index);
}


/** \brief Get number of type-descriptions in library.
 */
UINT TypeLib::GetTypeInfoCount() const
{
    return count();
}


/** \brief Get type info for member at index.
 */
TypeInfo TypeLib::GetTypeInfo(const UINT index) const
{
    return info(index);
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
TypeDesc TypeAttr::alias() const
{
    return TypeDesc(attr->tdescAlias);
}


/** \brief Get IDL attributes of type.
 */
IdlDesc TypeAttr::idl() const
{
    return IdlDesc(attr->idldescType);
}


/** \brief Get MEMBERID of constructor from TYPEATTR.
 */
MEMBERID TypeAttr::memidConstructor() const
{
    return constructor();
}


/** \brief Get MEMBERID of destructor from TYPEATTR.
 */
MEMBERID TypeAttr::memidDestructor() const
{
    return destructor();
}


/** \brief Get size of type instance from TYPEATTR.
 */
ULONG TypeAttr::cbSizeInstance() const
{
    return size();
}


/** \brief Get type kind from TYPEATTR.
 */
TYPEKIND TypeAttr::typekind() const
{
    return kind();
}


/** \brief Get member function count from TYPEATTR.
 */
WORD TypeAttr::cFuncs() const
{
    return functions();
}


/** \brief Get member variable count from TYPEATTR.
 */
WORD TypeAttr::cVars() const
{
    return variables();
}


/** \brief Get interface count from TYPEATTR.
 */
WORD TypeAttr::cImplTypes() const
{
    return interfaces();
}


/** \brief Get size of virtual table from TYPEATTR.
 */
WORD TypeAttr::cbSizeVft() const
{
    return vtblSize();
}


/** \brief Get byte alignment from TYPEATTR.
 */
WORD TypeAttr::cbAlignment() const
{
    return alignment();
}


/** \brief Get type flags from TYPEATTR.
 */
WORD TypeAttr::wTypeFlags() const
{
    return flags();
}


/** \brief Get major version number from TYPEATTR.
 */
WORD TypeAttr::wMajorVerNum() const
{
    return major();
}


/** \brief Get minor version number from TYPEATTR.
 */
WORD TypeAttr::wMinorVerNum() const
{
    return minor();
}


/** \brief Get description of alias if kind() is TKIND_ALIAS.
 */
TypeDesc TypeAttr::tdescAlias() const
{
    return alias();
}


/** \brief Get IDL attributes of type.
 */
IdlDesc TypeAttr::idldescType() const
{
    return idl();
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


/** \brief Get major version number from TLIBATTR.
 */
WORD TypeLibAttr::wMajorVerNum() const
{
    return major();
}


/** \brief Get minor version number from TLIBATTR.
 */
WORD TypeLibAttr::wMinorVerNum() const
{
    return minor();
}


/** \brief Get library flags from TLIBATTR.
 */
WORD TypeLibAttr::wLibFlags() const
{
    return flags();
}


/** \brief Open VARDESC from ITypeInfo.
 */
VarDesc::VarDesc(const ITypeInfoPtr &info,
    const UINT index)
{
    open(info, index);
}

/** \brief Construct VARDESC from ITypeInfo.
 */
void VarDesc::open(const ITypeInfoPtr &info,
    const UINT index)
{
    if (info) {
        desc.reset(newVarDesc(info.get(), index), [info](VARDESC *ptr) {
            info->ReleaseVarDesc(ptr);
        });
        ppv = info;
    } else {
        desc.reset();
        ppv.reset();
    }
}


/** \brief Check if VarDesc is valid.
 */
VarDesc::operator bool() const
{
    return bool(desc);
}


/** \brief Variable member ID.
 */
MEMBERID VarDesc::id() const
{
    return desc->memid;
}


/** \brief Description of variable.
 */
ElemDesc VarDesc::element() const
{
    return ElemDesc(desc->elemdescVar);
}


/** \brief Reference to VARIANT (not null only if kind() is VAR_CONST).
 */
const VARIANT & VarDesc::variant() const
{
    assert(kind() == VAR_CONST);
    return *desc->lpvarValue;
}


/** \brief Variable flags.
 */
WORD VarDesc::flags() const
{
    return desc->wVarFlags;
}


/** \brief Variable kind.
 */
VARKIND VarDesc::kind() const
{
    return desc->varkind;
}


/** \brief Variable member ID.
 */
MEMBERID VarDesc::memid() const
{
    return id();
}


/** \brief Description of variable.
 */
ElemDesc VarDesc::elemdescVar() const
{
    return element();
}


/** \brief Reference to VARIANT (not null only if kind() is VAR_CONST).
 */
const VARIANT & VarDesc::lpvarValue() const
{
    return variant();
}


/** \brief Variable flags.
 */
WORD VarDesc::wVarFlags() const
{
    return flags();
}


/** \brief Variable kind.
 */
VARKIND VarDesc::varkind() const
{
    return kind();
}


/** \brief Open FUNCDESC from ITypeInfo.
 */
FuncDesc::FuncDesc(const ITypeInfoPtr &info,
    const UINT index)
{
    open(info, index);
}

/** \brief Construct FUNCDESC from ITypeInfo.
 */
void FuncDesc::open(const ITypeInfoPtr &info,
    const UINT index)
{
    if (info) {
        desc.reset(newFuncDesc(info.get(), index), [info](FUNCDESC *ptr) {
            info->ReleaseFuncDesc(ptr);
        });
        ppv = info;
    } else {
        desc.reset();
        ppv.reset();
    }
}


/** \brief Check if FuncDesc is valid.
 */
FuncDesc::operator bool() const
{
    return bool(desc);
}


/** \brief Function member ID.
 */
MEMBERID FuncDesc::id() const
{
    return desc->memid;
}


/** \brief Function kind (static, virtual, dispatch-only).
 */
FUNCKIND FuncDesc::kind() const
{
    return desc->funckind;
}


/** \brief Invocation type (propertyget, propertyput, function).
 */
INVOKEKIND FuncDesc::invocation() const
{
    return desc->invkind;
}


/** \brief Function decoration (__fastcall, etc.).
 */
CALLCONV FuncDesc::decoration() const
{
    return desc->callconv;
}


/** \brief Element description for function argument at index.
 */
ElemDesc FuncDesc::arg(const SHORT index) const
{
    return ElemDesc(desc->lprgelemdescParam[index]);
}


/** \brief Get argument count.
 */
SHORT FuncDesc::args() const
{
    return desc->cParams;
}


/** \brief Get optional argument count.
 */
SHORT FuncDesc::optional() const
{
    return desc->cParamsOpt;
}


/** \brief Get offset for virtual function in vtable.
 */
SHORT FuncDesc::offset() const
{
    return desc->oVft;
}


/** \brief Eelment description of return type.
 */
ElemDesc FuncDesc::returnType() const
{
    return ElemDesc(desc->elemdescFunc);
}


/** \brief Number of possible return values.
 */
SHORT FuncDesc::returnCount() const
{
    return desc->cScodes;
}


/** \brief Function flags.
 */
WORD FuncDesc::flags() const
{
    return desc->wFuncFlags;
}


/** \brief Function member ID.
 */
MEMBERID FuncDesc::memid() const
{
    return id();
}


/** \brief Function kind (static, virtual, dispatch-only).
 */
FUNCKIND FuncDesc::funckind() const
{
    return kind();
}


/** \brief Invocation type (propertyget, propertyput, function).
 */
INVOKEKIND FuncDesc::invkind() const
{
    return invocation();
}


/** \brief Function decoration (__fastcall, etc.).
 */
CALLCONV FuncDesc::callconv() const
{
    return decoration();
}


/** \brief Element description for function argument at index.
 */
ElemDesc FuncDesc::lprgelemdescParam(const SHORT index) const
{
    return arg(index);
}


/** \brief Get argument count.
 */
SHORT FuncDesc::cParams() const
{
    return args();
}


/** \brief Get optional argument count.
 */
SHORT FuncDesc::cParamsOpt() const
{
    return optional();
}


/** \brief Get offset for virtual function in vtable.
 */
SHORT FuncDesc::oVft() const
{
    return offset();
}


/** \brief Eelment description of return type.
 */
ElemDesc FuncDesc::elemdescFunc() const
{
    return returnType();
}


/** \brief Number of possible return values.
 */
SHORT FuncDesc::cScodes() const
{
    return returnCount();
}


/** \brief Function flags.
 */
WORD FuncDesc::wFuncFlags() const
{
    return flags();
}


/** \brief Initialize from copied typedesc.
 */
TypeDesc::TypeDesc(const TYPEDESC &desc):
    desc(desc)
{}


/** \brief Initialize from moved typedesc.
 */
TypeDesc::TypeDesc(TYPEDESC &&desc):
    desc(std::move(desc))
{}


/** \brief Return variable type.
 */
VARTYPE TypeDesc::vt() const
{
    return desc.vt;
}


/** \brief Get description for pointer type.
 */
TypeDesc TypeDesc::pointer() const
{
    assert(vt() == VT_PTR || vt() == VT_SAFEARRAY);
    return TypeDesc(*desc.lptdesc);
}


/** \brief Get reference for type info.
 */
HREFTYPE TypeDesc::reference() const
{
    assert(vt() == VT_USERDEFINED);
    return desc.hreftype;
}


/** \brief Get description for array type.
 */
ArrayDesc TypeDesc::array() const
{
    assert(vt() == VT_CARRAY);
    return ArrayDesc(*desc.lpadesc);
}


/** \brief Get description for pointer type.
 */
TypeDesc TypeDesc::lptdesc() const
{
    return pointer();
}


/** \brief Get reference for type info.
 */
HREFTYPE TypeDesc::hreftype() const
{
    return reference();
}


/** \brief Get description for array type.
 */
ArrayDesc TypeDesc::lpadesc() const
{
    return array();
}


/** \brief Initialize from copied arraydesc.
 */
ArrayDesc::ArrayDesc(const ARRAYDESC &desc):
    desc(desc)
{}


/** \brief Initialize from moved arraydesc.
 */
ArrayDesc::ArrayDesc(ARRAYDESC &&desc):
    desc(std::move(desc))
{}


/** \brief Get type description.
 */
TypeDesc ArrayDesc::type() const
{
    return TypeDesc(desc.tdescElem);
}


/** \brief Get number of array dimensions.
 */
USHORT ArrayDesc::count() const
{
    return desc.cDims;
}


/** \brief Get dimension at index.
 */
SafeArrayBound ArrayDesc::bound(const USHORT index) const
{
    return SafeArrayBound(desc.rgbounds[index]);
}


/** \brief Get type description.
 */
TypeDesc ArrayDesc::tdescElem() const
{
    return type();
}


/** \brief Get number of array dimensions.
 */
USHORT ArrayDesc::cDims() const
{
    return count();
}


/** \brief Get dimension at index.
 */
SafeArrayBound ArrayDesc::rgbounds(const USHORT index) const
{
    return bound(index);
}


/** \brief Initialize from copied idlesc.
 */
IdlDesc::IdlDesc(const IDLDESC &desc):
    desc(desc)
{}


/** \brief Initialize from moved idlesc.
 */
IdlDesc::IdlDesc(IDLDESC &&desc):
    desc(std::move(desc))
{}


/** \brief Initialize from copied elemdesc.
 */
ElemDesc::ElemDesc(const ELEMDESC &desc):
    desc(desc)
{}


/** \brief Initialize from moved elemdesc.
 */
ElemDesc::ElemDesc(ELEMDESC &&desc):
    desc(std::move(desc))
{}


/** \brief Get type description.
 */
TypeDesc ElemDesc::type() const
{
    return TypeDesc(desc.tdesc);
}


/** \brief Get IDL description.
 */
IdlDesc ElemDesc::idl() const
{
    return IdlDesc(desc.idldesc);
}


/** \brief Get param description.
 */
ParamDesc ElemDesc::param() const
{
    return ParamDesc(desc.paramdesc);
}


/** \brief Get type description.
 */
TypeDesc ElemDesc::tdesc() const
{
    return type();
}


/** \brief Get IDL description.
 */
IdlDesc ElemDesc::idldesc() const
{
    return idl();
}


/** \brief Get param description.
 */
ParamDesc ElemDesc::paramdesc() const
{
    return param();
}


/** \brief Initialize from copied paramdesc.
 */
ParamDesc::ParamDesc(const PARAMDESC &desc):
    desc(desc)
{}


/** \brief Initialize from moved paramdesc.
 */
ParamDesc::ParamDesc(PARAMDESC &&desc):
    desc(std::move(desc))
{}


/** \brief Get size of default parameter.
 */
ULONG ParamDesc::size() const
{
    assert(flags() & PARAMFLAG_FHASDEFAULT);
    return desc.pparamdescex->cBytes;
}


/** \brief Get value of default parameter.
 */
VARIANTARG ParamDesc::value() const
{
    assert(flags() & PARAMFLAG_FHASDEFAULT);
    return desc.pparamdescex->varDefaultValue;
}


/** \brief Get flags for parameter.
 */
USHORT ParamDesc::flags() const
{
    return desc.wParamFlags;
}


/** \brief Get size of default parameter.
 */
ULONG ParamDesc::cBytes() const
{
    return size();
}


/** \brief Get value of default parameter.
 */
VARIANTARG ParamDesc::varDefaultValue() const
{
    return value();
}


/** \brief Get flags for parameter.
 */
USHORT ParamDesc::wParamFlags() const
{
    return flags();
}

}   /* autocom */
