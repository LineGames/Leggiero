package com.leggiero.Engine.Modules.FileSystem;

import android.content.ContextWrapper;
import android.content.res.AssetManager;
import android.os.Environment;
import android.os.storage.StorageManager;
import java.io.File;
import java.io.InputStream;
import java.util.Arrays;
import java.util.ArrayList;
import java.util.UUID;
import com.leggiero.Engine.LeggieroActivity;
import com.leggiero.Engine.LeggieroUtils;


public class FileSystemHelper {

	protected static ContextWrapper m_globalContext;
	
	static {
		LeggieroUtils.LoadNativeLibraries();
	}
	
	// Modules/FileSystem/_Internal/AndroidFileSystemJNIInterface.cpp
	private native void InitializeBundleAsset(Object assetManager);

	private native void SetIsExternalStorageAvailable(boolean isAvailable);
	private native void SetDataPaths(String mainDataPath, String rawDataPath, String cachePath, String tmpPath);
	private native void SetExternalDataPaths(String mainDataPath, String rawDataPath, String cachePath);
	
	public FileSystemHelper()
	{
	}
	
	public void InitializeHelper(ContextWrapper cw)
	{
		m_globalContext = cw;

		InitializeBundleAsset(cw.getAssets());

		boolean isExternalStorageAvailable = IsExternalStorageWritable();
		SetIsExternalStorageAvailable(isExternalStorageAvailable);

		// Initialize Internal Storages
		String mainDataPath = PathCombine(cw.getFilesDir().getPath(), "ldmaini");
		String rawDataPath = PathCombine(cw.getFilesDir().getPath(), "ldrawi");
		String cachePath = PathCombine(cw.getCacheDir().getPath(), "ldcachei");
		String tmpPath = PathCombine(cw.getCacheDir().getPath(), "ldtmp");

		CheckAndMakeDir(mainDataPath);
		CheckAndMakeDir(rawDataPath);
		CheckAndMakeDir(cachePath);
		CheckAndMakeDir(tmpPath);

		SetDataPaths(mainDataPath, rawDataPath, cachePath, tmpPath);

		// Initialize External Storages
		File externalFilesDir = null;
		File externalCacheDir = null;
		if (isExternalStorageAvailable) {
			externalFilesDir = cw.getExternalFilesDir(null);
			if (!externalFilesDir.canWrite()) {
				externalFilesDir = null;
			}
			externalCacheDir = cw.getExternalCacheDir();
			if (!externalCacheDir.canWrite()) {
				externalCacheDir = null;
			}
		}

		String externalMainDataPath;
		if (externalFilesDir == null)
		{
			externalMainDataPath = PathCombine(cw.getFilesDir().getPath(), "ldmaine");
		}
		else
		{
			externalMainDataPath = PathCombine(externalFilesDir.getPath(), "ldmaine");
		}

		String externalRawDataPath;
		if (externalFilesDir == null)
		{
			externalRawDataPath = PathCombine(cw.getFilesDir().getPath(), "ldrawe");
		}
		else
		{
			externalRawDataPath = PathCombine(externalFilesDir.getPath(), "ldrawe");
		}

		String externalCachePath;
		if (externalFilesDir == null)
		{
			externalCachePath = PathCombine(cw.getCacheDir().getPath(), "ldcachee");
		}
		else
		{
			externalCachePath = PathCombine(externalFilesDir.getPath(), "ldcachee");
		}
		
		CheckAndMakeDir(externalMainDataPath);
		CheckAndMakeDir(externalRawDataPath);
		CheckAndMakeDir(externalCachePath);

		SetExternalDataPaths(externalMainDataPath, externalRawDataPath, externalCachePath);
	}
	
	public static String PathCombine(String path1, String path2)
	{
	    File file1 = new File(path1);
	    File file2 = new File(file1, path2);
	    return file2.getPath();
	}

	public static void CheckAndMakeDir(String path)
	{
		File targetDir = new File(path);
		if (!targetDir.exists() || !targetDir.isDirectory())
		{
			targetDir.mkdirs();
		}
	}

	private static boolean IsExternalStorageWritable() {
		return Environment.getExternalStorageState() == Environment.MEDIA_MOUNTED;
	}

	private static long GetFreeSpaceInternal() {
		try {
			StorageManager storageManager = LeggieroActivity.m_this.getApplicationContext().getSystemService(StorageManager.class);
			UUID appSpecificInternalDirUuid = storageManager.getUuidForPath(LeggieroActivity.m_this.getFilesDir());
			return storageManager.getAllocatableBytes(appSpecificInternalDirUuid);
		}
		catch (Exception e)
		{
		}
		return 0;
	}

	private static long GetFreeSpaceExternal() {
		try {
			if (!IsExternalStorageWritable()) {
				return 0;
			}
			File externalDir = LeggieroActivity.m_this.getExternalFilesDir(null);
			if (externalDir == null) {
				return 0;
			}
			StorageManager storageManager = LeggieroActivity.m_this.getApplicationContext().getSystemService(StorageManager.class);
			UUID appSpecificInternalDirUuid = storageManager.getUuidForPath(externalDir);
			return storageManager.getAllocatableBytes(appSpecificInternalDirUuid);
		}
		catch (Exception e)
		{
		}
		return 0;
	}

	public static int CheckIsDirectory(String path)
	{
		File targetDir = new File(path);
		if (targetDir.isDirectory())
		{
			return 1;
		}
		return 0;
	}

	public static ArrayList<String> ListSubFiles(String path, int isListDirectory)
	{
		boolean isListDirectoryValue = (isListDirectory == 1);

		File targetDir = new File(path);
		if (!targetDir.isDirectory())
		{
			return null;
		}

		ArrayList<String> resultList = new ArrayList<String>();
		File[] subFileList = targetDir.listFiles();
		for (File currentFile : subFileList)
		{
			if (currentFile.isDirectory())
			{
				if (isListDirectoryValue)
				{
					resultList.add(currentFile.getName());
				}
			}
			else
			{
				if (!isListDirectoryValue)
				{
					resultList.add(currentFile.getName());
				}
			}
		}

		return resultList;
	}

	public static ArrayList<String> ListInAssetPath(String path)
	{
		if (m_globalContext == null)
		{
			return null;
		}

		AssetManager am = m_globalContext.getAssets();
		if (am == null)
		{
			return null;
		}

		String[] subItemList;
		try {
			subItemList = am.list(path);
		}
		catch (Exception ex) {
			return null;
		}
		if (subItemList == null)
		{
			return null;
		}

		return new ArrayList<String>(Arrays.asList(subItemList));
	}
}
