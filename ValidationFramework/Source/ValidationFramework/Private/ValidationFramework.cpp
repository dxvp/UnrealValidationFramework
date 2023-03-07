/**
Copyright Epic Games, Inc. All Rights Reserved.
Copyright 2022 Netflix, Inc.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include "ValidationFramework.h"
#include "AssetTypeActions_Base.h"
#include "ClassIconFinder.h"
#include "EditorUtilityWidget.h"
#include "VFProjectSettingsEditor.h"
#include "ISettingsModule.h"
#include "Engine.h"
#include "LevelEditor.h"
#include "Templates/SharedPointer.h"
#include "Runtime/Core/Public/Modules/ModuleManager.h"
#include "Editor/Blutility/Classes/EditorUtilityWidgetBlueprint.h"
#include "EditorAssetLibrary.h"
#include "EditorUtilitySubsystem.h"
#include "Interfaces/IPluginManager.h"
#include "ToolMenu.h"

#define LOCTEXT_NAMESPACE "FValidationFrameworkModule"

void FValidationFrameworkModule::StartupModule()
{
	// Add Netflix Style Sheet
	const FString PluginContentDir = IPluginManager::Get().FindPlugin(TEXT("ValidationFramework"))->GetBaseDir();
	StyleSet = MakeShareable(new FSlateStyleSet("NetflixStyleSheet"));
	
	StyleSet->SetContentRoot(PluginContentDir);
	StyleSet->SetCoreContentRoot(PluginContentDir);
		
	FSlateStyleRegistry::RegisterSlateStyle(*StyleSet.Get());
	
	// Add & Register Project Settings
	if(ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->RegisterSettings("Project", "Plugins", "Validation Framework Settings",
		LOCTEXT("RuntimeSettingsName", "Validation Framework Settings"),
		 LOCTEXT("RuntimeSettingsDescription", "Validation Framework Settings"),
			GetMutableDefault<UVFProjectSettingsEditor>());
	}

	// Add Dexter tools menu
	{
		UToolMenu* menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu");
		FToolMenuSection& section = menu->FindOrAddSection(NAME_None);

		FToolMenuEntry& build_entry =
			section.AddSubMenu(
				"Dexter",
				FText::FromString("Dexter Tools"),
				FText::FromString("Dexter Tools Tooltip"),
				FNewToolMenuChoice()
			);

		build_entry.InsertPosition = FToolMenuInsert("Help", EToolMenuInsertType::Before);

		static const FName dexter_menu_name = "LevelEditor.MainMenu.Dexter";	
		UToolMenu* dexter_menu = UToolMenus::Get()->RegisterMenu(dexter_menu_name);

		FToolMenuSection& dexter_menu_section = dexter_menu->AddSection(TEXT("Stage Tools"), LOCTEXT("Stage Tools", "Stage Tools"));
		FToolMenuEntry& build_entry_asd = dexter_menu_section.AddMenuEntry(
			"Validation Framework",
			FText::FromString("Open the validation framework UI"),
			FText(),
			FSlateIcon(),
			FUIAction(FExecuteAction::CreateRaw(this, &FValidationFrameworkModule::OpenValidationFrameworkCB))
		);
	}
}

void FValidationFrameworkModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module

	if(ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->UnregisterSettings("Project", "Plugins", "Validation Framework Settings");
	}

	// Remove Style
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleSet.Get());
	ensure(StyleSet.IsUnique());
	StyleSet.Reset();
}

void FValidationFrameworkModule::AddMenuEntry(FMenuBuilder& MenuBuilder) const
{
	// Create Section
	MenuBuilder.BeginSection("Netflix Tools", TAttribute<FText>(FText::FromString("Netflix")));
	{
		// Create a Submenu inside of the Section
		MenuBuilder.AddSubMenu(FText::FromString("Netflix"),
			FText::FromString("Collection Of Netflix Tools"),
			FNewMenuDelegate::CreateRaw(this, &FValidationFrameworkModule::FillSubmenu),
			false);
	}
	MenuBuilder.EndSection();
}

void FValidationFrameworkModule::FillSubmenu(FMenuBuilder& MenuBuilder) const
{
	// Create the Submenu Entries
	MenuBuilder.AddMenuEntry(
		FText::FromString("Validation Framework"),
		FText::FromString("Open the validation framework UI"),
		FSlateIcon(),
		FUIAction(FExecuteAction::CreateRaw(this, &FValidationFrameworkModule::OpenValidationFrameworkCB))
	);
}

void FValidationFrameworkModule::OpenValidationFrameworkCB() const
{
    UObject * Blueprint = UEditorAssetLibrary::LoadAsset(
    	FString(TEXT("/ValidationFramework/Blueprints/VP_ValidationFramework_V2.VP_ValidationFramework_V2"))
    );

	if(IsValid(Blueprint))
	{
      UEditorUtilityWidgetBlueprint* EditorWidget = Cast<UEditorUtilityWidgetBlueprint>(Blueprint);
      if (IsValid(EditorWidget))
      {
        UEditorUtilitySubsystem* EditorUtilitySubsystem = GEditor->GetEditorSubsystem<UEditorUtilitySubsystem>();
        EditorUtilitySubsystem->SpawnAndRegisterTab(EditorWidget);
      }
    }
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FValidationFrameworkModule, ValidationFramework)