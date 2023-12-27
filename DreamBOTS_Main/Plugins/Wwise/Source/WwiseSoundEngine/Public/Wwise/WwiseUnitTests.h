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

#pragma once

#include "WwiseDefines.h"

// We use Catch2 Test Harness, available starting UE5.1
#if UE_5_2_OR_LATER
#include "Tests/TestHarnessAdapter.h"
#elif UE_5_1_OR_LATER
#include "Tests/TestHarness.h"
#endif // UE_5_2_OR_LATER

#if UE_5_1_OR_LATER
#if WITH_LOW_LEVEL_TESTS || defined(WITH_AUTOMATION_TESTS) && WITH_AUTOMATION_TESTS

#define WWISE_UNIT_TESTS 1

#endif
#endif // UE_5_1_OR_LATER

#ifndef WWISE_UNIT_TESTS
#define WWISE_UNIT_TESTS 0
#endif

#if WWISE_UNIT_TESTS
#include <type_traits>

// Add logging facilities when running in Automation
#if defined(WITH_AUTOMATION_TESTS) && WITH_AUTOMATION_TESTS
#define WWISETEST_LOG(Format, ...) FAutomationTestFramework::Get().GetCurrentTest()->AddInfo(FString::Printf(TEXT(Format), __VA_ARGS__));
#else
#define WWISETEST_LOG(Format, ...) (void)0
#endif

#endif // WWISE_UNIT_TESTS