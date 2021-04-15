using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;

namespace ProjectCreatorCore
{
    /// <summary>
    /// Project template list data
    /// </summary>
    internal class TemplateList
    {
        public class PathData
        {
            public List<string> pathList = new List<string>();

            public static PathData ParsePathString(string pathString)
            {
                if (string.IsNullOrWhiteSpace(pathString))
                {
                    return null;
                }

                string[] pathTokens = pathString.Split('/');
                PathData data = new PathData();

                foreach (string currentToken in pathTokens)
                {
                    if (string.IsNullOrWhiteSpace(currentToken))
                    {
                        continue;
                    }
                    if (currentToken == "*")
                    {
                        continue;
                    }
                    data.pathList.Add(currentToken.Trim());
                }

                if (data.pathList.Count == 0)
                {
                    // Not supporing just root copy
                    return null;
                }

                return data;
            }

            public string CombineWith(string basePath)
            {
                string currentPath = basePath;
                foreach (string currentName in pathList)
                {
                    currentPath = Path.Combine(currentPath, currentName);
                }
                return currentPath;
            }
        }

        public class WorkSet
        {
            public List<Tuple<PathData, PathData>> copyEntries = new List<Tuple<PathData, PathData>>();
            public List<Tuple<PathData, PathData>> filterEntries = new List<Tuple<PathData, PathData>>();
            public List<Tuple<PathData, PathData>> copyAllEntries = new List<Tuple<PathData, PathData>>();

            public void DoCopyWorks(string templateDataRoot, string targetProjectRoot, FilterProcessor filter)
            {
                foreach (Tuple<PathData, PathData> currentEntry in copyAllEntries)
                {
                    _CopyAllIn(currentEntry.Item1.CombineWith(templateDataRoot), currentEntry.Item2.CombineWith(targetProjectRoot));
                }

                foreach (Tuple<PathData, PathData> currentEntry in copyEntries)
                {
                    string sourceFilePath = currentEntry.Item1.CombineWith(templateDataRoot);
                    if (!File.Exists(sourceFilePath))
                    {
                        Console.Error.WriteLine("  [W] Template project data file not exists: {0}", sourceFilePath);
                        continue;
                    }
                    string targetFilePath = currentEntry.Item2.CombineWith(targetProjectRoot);
                    try
                    {
                        string parentDir = Directory.GetParent(targetFilePath).FullName;
                        Directory.CreateDirectory(parentDir);
                        File.Copy(sourceFilePath, targetFilePath);
                    }
                    catch
                    {
                        Console.Error.WriteLine("  [W] An error occured during copy {0} to {1}", sourceFilePath, targetFilePath);
                    }
                }

                var utf8WithoutBOM = new UTF8Encoding(false);
                foreach (Tuple<PathData, PathData> currentEntry in filterEntries)
                {
                    string sourceFilePath = currentEntry.Item1.CombineWith(templateDataRoot);
                    if (!File.Exists(sourceFilePath))
                    {
                        Console.Error.WriteLine("  [W] Template project data file not exists: {0}", sourceFilePath);
                        continue;
                    }

                    string fileContents = null;
                    try
                    {
                        fileContents = File.ReadAllText(sourceFilePath, Encoding.UTF8);
                    }
                    catch
                    {
                        Console.Error.WriteLine("  [W] An error occured during reading file data in {0}", sourceFilePath);
                        continue;
                    }

                    string filteredContents = filter.Filter(fileContents);

                    string targetFilePath = currentEntry.Item2.CombineWith(targetProjectRoot);
                    string fileExtension = Path.GetExtension(targetFilePath);

                    bool isNeedBOM = false;
                    switch (fileExtension.ToLower())
                    {
                        case ".cpp":
                        case ".h":
                            isNeedBOM = true;
                            break;
                    }

                    try
                    {
                        string parentDir = Directory.GetParent(targetFilePath).FullName;
                        Directory.CreateDirectory(parentDir);
                        if (isNeedBOM)
                        {
                            File.WriteAllText(targetFilePath, filteredContents, Encoding.UTF8);
                        }
                        else
                        {
                            File.WriteAllText(targetFilePath, filteredContents, utf8WithoutBOM);
                        }
                    }
                    catch
                    {
                        Console.Error.WriteLine("  [W] An error occured during writing to project file in {0}", targetFilePath);
                    }
                }
            }

            private void _CopyAllIn(string sourcePath, string targetPath)
            {
                if (!Directory.Exists(sourcePath))
                {
                    Console.Error.WriteLine("  [W] Template project data path not exists: {0}", sourcePath);
                    return;
                }

                try
                {
                    Directory.CreateDirectory(targetPath);
                }
                catch
                {
                    Console.Error.WriteLine("  [W] Cannot create project data directory: {0}", targetPath);
                }

                DirectoryInfo sourceDirectory = new DirectoryInfo(sourcePath);
                foreach (var subDirectory in sourceDirectory.EnumerateDirectories())
                {
                    _CopyAllIn(subDirectory.FullName, Path.Combine(targetPath, subDirectory.Name));
                }
                foreach (var subFile in sourceDirectory.EnumerateFiles())
                {
                    try
                    {
                        File.Copy(subFile.FullName, Path.Combine(targetPath, subFile.Name));
                    }
                    catch
                    {
                        Console.Error.WriteLine("  [W] An error occured during copy {0} to {1}", subFile.FullName, Path.Combine(targetPath, subFile.Name));
                    }
                }
            }
        }

        public WorkSet mainWorkSet = new WorkSet();
        public Dictionary<string, WorkSet> conditionalWorkSets = new Dictionary<string, WorkSet>();

        public static TemplateList ReadFile(string templateListFilePath, FilterProcessor contentsFilter)
        {
            if (string.IsNullOrWhiteSpace(templateListFilePath))
            {
                return null;
            }

            TemplateList listData = new TemplateList();

            try
            {
                using (FileStream fs = new FileStream(templateListFilePath, FileMode.Open, FileAccess.Read))
                using (TextReader rd = new StreamReader(fs))
                {
                    WorkSet currentTargetSet = listData.mainWorkSet;

                    string lineBuffer = rd.ReadLine();
                    while (lineBuffer != null)
                    {
                        if (string.IsNullOrWhiteSpace(lineBuffer) || lineBuffer.StartsWith('#'))
                        {
                            lineBuffer = rd.ReadLine();
                            continue;
                        }
                        lineBuffer = contentsFilter.Filter(lineBuffer).Trim();

                        string[] lineTokens = lineBuffer.Split(':', StringSplitOptions.TrimEntries);
                        if (lineTokens.Length > 0)
                        {
                            if (lineTokens[0].StartsWith('['))
                            {
                                // Section Descriptor
                                string sectionName = _PopSectionName(lineTokens[0]);
                                if (sectionName != null)
                                {
                                    sectionName = sectionName.Trim();
                                    if (sectionName == string.Empty)
                                    {
                                        currentTargetSet = listData.mainWorkSet;
                                    }
                                    else
                                    {
                                        if (!listData.conditionalWorkSets.ContainsKey(sectionName))
                                        {
                                            listData.conditionalWorkSets.Add(sectionName, new WorkSet());
                                        }
                                        currentTargetSet = listData.conditionalWorkSets[sectionName];
                                    }
                                }
                            }
                            else
                            {
                                switch (lineTokens[0])
                                {
                                    case "C":   // Copy
                                        {
                                            if (lineTokens.Length > 1)
                                            {
                                                PathData srcPathData = PathData.ParsePathString(lineTokens[1]);
                                                PathData targetPathData = null;
                                                if (lineTokens.Length > 2)
                                                {
                                                    targetPathData = PathData.ParsePathString(lineTokens[2]);
                                                }
                                                else
                                                {
                                                    targetPathData = srcPathData;
                                                }

                                                if (lineTokens[1].EndsWith("/*"))
                                                {
                                                    currentTargetSet.copyAllEntries.Add(new Tuple<PathData, PathData>(srcPathData, targetPathData));
                                                }
                                                else
                                                {
                                                    currentTargetSet.copyEntries.Add(new Tuple<PathData, PathData>(srcPathData, targetPathData));
                                                }
                                            }
                                        }
                                        break;

                                    case "F":   // Filter
                                        {
                                            if (lineTokens.Length > 1)
                                            {
                                                PathData srcPathData = PathData.ParsePathString(lineTokens[1]);
                                                PathData targetPathData = null;
                                                if (lineTokens.Length > 2)
                                                {
                                                    targetPathData = PathData.ParsePathString(lineTokens[2]);
                                                }
                                                else
                                                {
                                                    targetPathData = srcPathData;
                                                }
                                                currentTargetSet.filterEntries.Add(new Tuple<PathData, PathData>(srcPathData, targetPathData));
                                            }
                                        }
                                        break;
                                }
                            }
                        }

                        lineBuffer = rd.ReadLine();
                    }
                }
            }
            catch
            {
                return null;
            }

            return listData;
        }

        private static string _PopSectionName(string candidateToken)
        {
            if (string.IsNullOrWhiteSpace(candidateToken))
            {
                return null;
            }

            StringBuilder sb = new StringBuilder();
            sb.EnsureCapacity(candidateToken.Length);

            int pairCounter = 0;
            bool isCaptureStarted = false;
            for (int i = 0; i < candidateToken.Length; ++i)
            {
                char currentChar = candidateToken[i];
                switch (currentChar)
                {
                    case '[':
                        {
                            if (isCaptureStarted)
                            {
                                sb.Append(currentChar);
                            }
                            ++pairCounter;
                            isCaptureStarted = true;
                        }
                        break;

                    case ']':
                        {
                            if (pairCounter > 1)
                            {
                                --pairCounter;
                            }
                            else if (pairCounter == 1)
                            {
                                return sb.ToString();
                            }

                            if (isCaptureStarted)
                            {
                                sb.Append(currentChar);
                            }
                        }
                        break;

                    default:
                        {
                            if (isCaptureStarted)
                            {
                                sb.Append(currentChar);
                            }
                        }
                        break;
                }
            }

            return null;
        }
    }
}
