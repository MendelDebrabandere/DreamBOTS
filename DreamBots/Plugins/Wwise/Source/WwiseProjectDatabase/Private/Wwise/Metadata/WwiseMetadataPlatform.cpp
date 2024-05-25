/*******************************************************************************
The content of this file includes portions of the proprietary AUDIOKINETIC Wwise
Technology released in source code form as part of the game integration package.
The content of this file may not be used without valid licenses to the
AUDIOKINETIC Wwise Technology.
Note that the use of the game engine is subject to the Unreal(R) Engine End User
License Agreement at https://www.unrealengine.com/en-US/eula/unreal
 
License Usage
 
Licensees holding valid licenses to the AUDIOKINETIC Wwise Technology may use
this file in accordance with the end user license agreement provided with the
software or, alternatively, in accordance with the terms contained
in a written agreement between you and Audiokinetic Inc.
Copyright (c) 2023 Audiokinetic Inc.
*******************************************************************************/

#include "Wwise/Metadata/WwiseMetadataPlatform.h"
#include "Wwise/Metadata/WwiseMetadataLoader.h"
#include "Wwise/Stats/ProjectDatabase.h"

FWwiseMetadataPlatformAttributes::FWwiseMetadataPlatformAttributes()
{
	UE_LOG(LogWwiseProjectDatabase, Error, TEXT("Using default FWwiseMetadataPlatformAttributes"));
}

FWwiseMetadataPlatformAttributes::FWwiseMetadataPlatformAttributes(FWwiseMetadataLoader& Loader) :
	Name(Loader.GetString(this, TEXT("Name"))),
	BasePlatform(Loader.GetString(this, TEXT("BasePlatform"))),
	Generator(Loader.GetString(this, TEXT("Generator")))
{
	Loader.LogParsed(TEXT("PlatformAttributes"), 0, Name);
}

FWwiseMetadataPlatformReference::FWwiseMetadataPlatformReference(FWwiseMetadataLoader& Loader) :
	Name(Loader.GetString(this, TEXT("Name"))),
	GUID(Loader.GetGuid(this, TEXT("GUID"))),
	BasePlatform(Loader.GetString(this, TEXT("BasePlatform"))),
	BasePlatformGUID(Loader.GetGuid(this, TEXT("BasePlatformGUID"))),
	Path(Loader.GetString(this, TEXT("Path")))
{
	Loader.LogParsed(TEXT("PlatformReference"), 0, Name);
}

FWwiseMetadataPlatform::FWwiseMetadataPlatform()
{
	UE_LOG(LogWwiseProjectDatabase, Error, TEXT("Using default FWwiseMetadataPlatform"));
}

FWwiseMetadataPlatform::FWwiseMetadataPlatform(FWwiseMetadataLoader& Loader) :
	FWwiseMetadataPlatformAttributes(Loader)
{
	Loader.LogParsed(TEXT("Platform"), 0, Name);
}
