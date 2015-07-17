#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "iappsystem.h"

#define OUT_Z_CAP(x)

typedef void *FileHandle_t;
enum FileSystemSeek_t;
class CUtlBuffer;
typedef void *FSAllocFunc_t;

#define BASEFILESYSTEM_INTERFACE_VERSION		"VBaseFileSystem011"


class IBaseFileSystem
{
public:
	virtual int				Read(void* pOutput, int size, FileHandle_t file) = 0;
	virtual int				Write(void const* pInput, int size, FileHandle_t file) = 0;

	// if pathID is NULL, all paths will be searched for the file
	virtual FileHandle_t	Open(const char *pFileName, const char *pOptions, const char *pathID = 0) = 0;
	virtual void			Close(FileHandle_t file) = 0;


	virtual void			Seek(FileHandle_t file, int pos, FileSystemSeek_t seekType) = 0;
	virtual unsigned int	Tell(FileHandle_t file) = 0;
	virtual unsigned int	Size(FileHandle_t file) = 0;
	virtual unsigned int	Size(const char *pFileName, const char *pPathID = 0) = 0;

	virtual void			Flush(FileHandle_t file) = 0;
	virtual bool			Precache(const char *pFileName, const char *pPathID = 0) = 0;

	virtual bool			FileExists(const char *pFileName, const char *pPathID = 0) = 0;
	virtual bool			IsFileWritable(char const *pFileName, const char *pPathID = 0) = 0;
	virtual bool			SetFileWritable(char const *pFileName, bool writable, const char *pPathID = 0) = 0;

	virtual long			GetFileTime(const char *pFileName, const char *pPathID = 0) = 0;

	//--------------------------------------------------------
	// Reads/writes files to utlbuffers. Use this for optimal read performance when doing open/read/close
	//--------------------------------------------------------
	virtual bool			ReadFile(const char *pFileName, const char *pPath, CUtlBuffer &buf, int nMaxBytes = 0, int nStartingByte = 0, FSAllocFunc_t pfnAlloc = 0) = 0;
	virtual bool			WriteFile(const char *pFileName, const char *pPath, CUtlBuffer &buf) = 0;
	virtual bool			UnzipFile(const char *pFileName, const char *pPath, const char *pDestination) = 0;
};

#define FILESYSTEM_INTERFACE_VERSION			"VFileSystem022"

class IFileSystem : public IAppSystem, public IBaseFileSystem
{
public:
	//--------------------------------------------------------
	// Steam operations
	//--------------------------------------------------------

	virtual bool			IsSteam() const = 0;

	// Supplying an extra app id will mount this app in addition 
	// to the one specified in the environment variable "steamappid"
	// 
	// If nExtraAppId is < -1, then it will mount that app ID only.
	// (Was needed by the dedicated server b/c the "SteamAppId" env var only gets passed to steam.dll
	// at load time, so the dedicated couldn't pass it in that way).
	virtual	int MountSteamContent(int nExtraAppId = -1) = 0;

	//--------------------------------------------------------
	// Search path manipulation
	//--------------------------------------------------------

	// Add paths in priority order (mod dir, game dir, ....)
	// If one or more .pak files are in the specified directory, then they are
	//  added after the file system path
	// If the path is the relative path to a .bsp file, then any previous .bsp file 
	//  override is cleared and the current .bsp is searched for an embedded PAK file
	//  and this file becomes the highest priority search path ( i.e., it's looked at first
	//   even before the mod's file system path ).
	virtual void			AddSearchPath(const char *pPath, const char *pathID, int addType = 1) = 0;
	virtual bool			RemoveSearchPath(const char *pPath, const char *pathID = 0) = 0;

	// Remove all search paths (including write path?)
	virtual void			RemoveAllSearchPaths(void) = 0;

	// Remove search paths associated with a given pathID
	virtual void			RemoveSearchPaths(const char *szPathID) = 0;

	// This is for optimization. If you mark a path ID as "by request only", then files inside it
	// will only be accessed if the path ID is specifically requested. Otherwise, it will be ignored.
	// If there are currently no search paths with the specified path ID, then it will still
	// remember it in case you add search paths with this path ID.
	virtual void			MarkPathIDByRequestOnly(const char *pPathID, bool bRequestOnly) = 0;

	// converts a partial path into a full path
	// Prefer using the RelativePathToFullPath_safe template wrapper to calling this directly
	virtual const char		*RelativePathToFullPath(const char *pFileName, const char *pPathID, OUT_Z_CAP(maxLenInChars) char *pDest, int maxLenInChars, int pathFilter = 0, void *pPathType = 0) = 0;


	// Returns the search path, each path is separated by ;s. Returns the length of the string returned
	// Prefer using the GetSearchPath_safe template wrapper to calling this directly
	virtual int				GetSearchPath(const char *pathID, bool bGetPackFiles, OUT_Z_CAP(maxLenInChars) char *pDest, int maxLenInChars) = 0;


	// interface for custom pack files > 4Gb
	virtual bool			AddPackFile(const char *fullpath, const char *pathID) = 0;

	//--------------------------------------------------------
	// File manipulation operations
	//--------------------------------------------------------

	// Deletes a file (on the WritePath)
	virtual void			RemoveFile(char const* pRelativePath, const char *pathID = 0) = 0;

	// Renames a file (on the WritePath)
	virtual bool			RenameFile(char const *pOldPath, char const *pNewPath, const char *pathID = 0) = 0;

	// create a local directory structure
	virtual void			CreateDirHierarchy(const char *path, const char *pathID = 0) = 0;

	// File I/O and info
	virtual bool			IsDirectory(const char *pFileName, const char *pathID = 0) = 0;

};

#endif // FILESYSTEM_H