/*
 * Copyright (C) 2013-2014  LINK/2012 <dma_2012@hotmail.com>
 * Licensed under GNU GPL v3, see LICENSE at top level directory.
 * 
 */
#ifndef ASI_H
#define	ASI_H

#include <windows.h>    // for HMODULE
#include <string>
#include <list>
#include <vector>
#include <modloader.hpp>
#include <modloader_util_path.hpp>
using namespace modloader;

// Forward path_translator_base from args_translator.h
struct path_translator_base;

// Cleo iVersion for scripts
enum
{
    CLEO_VER_NONE       = '\0',
    CLEO_VER_3          = '3',
    CLEO_VER_4          = '4',
    CLEO_VER_5          = '5',
};


/*
 *  The plugin object
 */
extern class CThePlugin* asiPlugin;
class CThePlugin : public modloader::CPlugin
{
    public:

        //
        const char* GetName();
        const char* GetAuthor();
        const char* GetVersion();
        bool OnStartup();
        bool OnShutdown();
        bool CheckFile(modloader::ModLoaderFile& file);
        bool ProcessFile(const modloader::ModLoaderFile& file);
        bool PosProcess();
        const char** GetExtensionTable();
        
        /*
         *  Information about asi plugins (and more)
         */
        struct ModuleInfo
        {
            bool bIsASI;                // Is this a ASI module?
            bool bIsD3D9;               // Is this a D3D9.dll module?
            bool bIsMainExecutable;     // Is this the main executable? (gta_sa.exe etc)
            bool bIsCleo;               // Is the main CLEO.asi or any .cleo plugin
            bool bIsMainCleo;           // Is the main CLEO.asi
            
            struct      // Hacks that some ASIs will need to work properly
            {
                bool bRyosukeModuleName;
                
            } hacks;
            
            HMODULE module;         // The module handle
            std::string name;       // The asi filename, like "bbb.asi"
            std::string path;       // The asi path, like "modloader/aaa/bbb.asi"
            std::string folder;     // The folder where the asi is at, like "modloader/aaa/"
            std::vector<path_translator_base*> translators;

            

            ModuleInfo(std::string&& path);
            bool Load();            // Loads the module
            void Free();            // Frees the module
            
            void PatchImports();    // Patches IAT to translate arguments such as path
            void RestoreImports();  // Unpatches the IAT
            
            // Find translator for @symbol and @libname on translators list
            path_translator_base* FindTranslatorFrom(const char* symbol, const char* libname);
        };
        
        /*
         *  Information about cleo scripts 
         */
        struct CsInfo
        {
            char        iVersion;
            bool        bIsMission;
            
            std::string path;       // e.g. "modloader/foo/script.cs"
            std::string folder;     // e.g. "modloader/foo/"
            
            CsInfo(const modloader::ModLoaderFile& file);
            static bool GetVersionFromExtension(const char* ext, char& version);
        };
        
        
        
        /*
         *  Finds a ModuleInfo from an address that's supposed to be inside it 
         */
        ModuleInfo* FindModuleFromAddress(const void* addr)
        {
            // Find HMODULE by @addr
            HMODULE hModule;
            if(GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           (char*)addr, &hModule))
            {
                // Find the ModuleInfo in our list of modules
                for(auto& asi : this->asiList)
                    if(asi.module == hModule) return &asi;
            }
            
            // Not found
            return nullptr;
        }

        
        typedef std::list<ModuleInfo>   ModuleInfoList;
        typedef std::list<CsInfo>       CsInfoList;
        
        // List of asi files need to load (or loaded)
        ModuleInfoList asiList;  // It's called asiList but it's not limited to .asi files!
        
        // List of CLEO scripts (.cs, .cs3, .cs4, .cs5, .cm)
        CsInfoList     csList;   // It's called cs but it's not limited to .cs files (e.g. cm files works)
        
        

        
};

#endif	/* ASI_H */

