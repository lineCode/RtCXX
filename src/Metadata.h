#pragma once
#include "Fwd.h"
#include <string>
#include <any>
#include <unordered_map>

RTCXX_NAMESPACE_BEGIN

#define DECLARE_METADATA_STATIC_CAST_FLAG(Class) \
	constexpr static EMetadataCastFlags StaticCastFlags() { return EMetadataCastFlags(MCF_##Class); }

#define DECLARE_METADATA(Class, SuperClass)                                   \
public:                                                                       \
	DECLARE_METADATA_STATIC_CAST_FLAG(Class)                                  \
	FORCEINLINE Class()                                                                   \
		: SuperClass()                                                        \
	{                                                                         \
		CastFlags = EMetadataCastFlags(CastFlags | Class::StaticCastFlags()); \
	}                                                                         \
	FORCEINLINE Class(const std::string& InName)                                          \
		: SuperClass(InName)                                                  \
	{                                                                         \
		CastFlags = EMetadataCastFlags(CastFlags | Class::StaticCastFlags()); \
	}

enum EMetadataCastFlags
{
	MCF_None               = 0x00000000,
	MCF_CMetadata          = 0x00000001,
	MCF_CMetaFunction      = 0x00000002,
	MCF_CMetaClass         = 0x00000004,
	MCF_CMetaProperty      = 0x00000008,
	MCF_CBoolProperty      = 0x00000010,
	MCF_CNumericProperty   = 0x00000020,
	MCF_CI8Property        = 0x00000040,
	MCF_CI16Property       = 0x00000080,
	MCF_CI32Property       = 0x00000100,
	MCF_CI64Property       = 0x00000200,
	MCF_CU8Property        = 0x00000400,
	MCF_CU16Property       = 0x00000800,
	MCF_CU32Property       = 0x00001000,
	MCF_CU64Property       = 0x00002000,
	MCF_CF32Property       = 0x00004000,
	MCF_CF64Property       = 0x00008000,
	MCF_CStrProperty       = 0x00010000,
	MCF_CClassProperty     = 0x00020000,
	MCF_CObjectProperty    = 0x00040000,
	MCF_CPtrProperty       = 0x00080000,
	MCF_CObjectPtrProperty = 0x00100000,
	MCF_CArrayProperty     = 0x00200000,
};

class RTCXX_API CMetadata
{
public:
	DECLARE_METADATA_STATIC_CAST_FLAG(CMetadata)

	template<typename T>
	T* CastTo()
	{
		if (GetCastFlags() & T::StaticCastFlags())
			return static_cast<T*>(this);
		return nullptr;
	}

public:
	CMetadata()
		: CastFlags(MCF_None)
	{
		CastFlags = EMetadataCastFlags(CastFlags | StaticCastFlags());
	}
	CMetadata(const std::string& InName)
		: Name(InName)
		, CastFlags(MCF_None)
	{
		CastFlags = EMetadataCastFlags(CastFlags | StaticCastFlags());
	}
	CMetadata(const CMetadata&) = delete;
	CMetadata(CMetadata&&) = delete;
	CMetadata& operator=(const CMetadata&) = delete;
	CMetadata& operator=(CMetadata&&) = delete;
	virtual ~CMetadata() {}

	std::string GetFullName()
	{
		std::string FullName = Name;
		CMetadata* Owner = GetOwner();
		while (Owner)
		{
			FullName = Owner->GetName() + "::" + FullName;
			Owner = Owner->GetOwner();
		}
		return FullName;
	}

	I32 GetId() const { return Id; }
	const std::string& GetName() const { return Name; }
	CMetadata* GetOwner() const { return Owner; }
	U64 GetCastFlags() { return CastFlags; }

	bool HasMetadata(const std::string& InName) const
	{
		return MetadataMap.find(InName) != MetadataMap.end();
	}

	std::any GetMetadata(const std::string& InName) const
	{
		auto FindIterator = MetadataMap.find(InName);
		if (FindIterator != MetadataMap.end())
		{
			return MetadataMap.find(InName)->second;
		}
		return std::any();
	}

	void SetMetadata(const std::string& InName, const std::any& InMetadata)
	{
		MetadataMap.insert_or_assign(InName, InMetadata);
	}

	void SetMetadata(const std::string& InName, std::any&& InMetadata)
	{
		MetadataMap.insert_or_assign(InName, std::move(InMetadata));
	}

public:
	void SetName(const std::string& InName) { Name = InName; }
	void SetOwner(CMetadata* InOwner) { Owner = InOwner; }

protected:
	I32 Id;
	std::string Name;
	CMetadata* Owner;
	std::unordered_map<std::string, std::any> MetadataMap;
	EMetadataCastFlags CastFlags;

private:
	friend class CController;
};

RTCXX_NAMESPACE_END