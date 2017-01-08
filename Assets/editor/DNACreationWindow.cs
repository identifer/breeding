using UnityEngine;
using UnityEditor;

public class DNACreationWindow : EditorWindow
{
    [MenuItem("繁殖/创建DNA")]
    public static void OpenDNACreationWindow()
    {
        //show prompt.
        DNACreationWindow window = ScriptableObject.CreateInstance<DNACreationWindow>();
        window.position = new Rect(0, 0, 200, 100);
        EditorEx.CenterOnMainWin(window);

        window.ShowPopup();
    }

    void CreatePrefab(string name)
    {
        //Create GameObject
        GameObject dnaGO = new GameObject(name);
        DNA dna = dnaGO.AddComponent<DNA>();
        dna.Name = name;
        Selection.activeGameObject = dnaGO;

        //make sure the directory is exist.
        if (!System.IO.Directory.Exists(DNA.PrefabDirectory))
        {
            System.IO.Directory.CreateDirectory(DNA.PrefabDirectory);
        }

        //create prefab.
        string PrefabPath = MakeFullPath(name);
        GameObject prefab = PrefabUtility.CreatePrefab(PrefabPath, dnaGO, ReplacePrefabOptions.Default);
        GameObject.DestroyImmediate(dnaGO);
        EditorGUIUtility.PingObject(prefab);
        Selection.activeGameObject = prefab;
    }

    string ValidateFileName(string name)
    {
        return name.Replace(' ', '_');
    }

    string MakeFullPath(string name)
    {
        return DNA.PrefabDirectory + ValidateFileName(name) + ".prefab";
    }

    bool CheckPrefabIsExist(string name)
    {
        return System.IO.File.Exists(MakeFullPath(name));
    }

    string DNAName = "New DNA";
    Rect windowRect = new Rect(1, 1, 199, 99);
    bool needClose = false;
    bool nameExist = false;

    DNACreationWindow()
    {
        nameExist = CheckPrefabIsExist(DNAName);
    }

    void OnGUI()
    {
        EditorGUI.BeginChangeCheck();
        BeginWindows();
        windowRect = GUI.Window(1, windowRect, DoWindow, "创建一个新的DNA");
        EndWindows();
        if (EditorGUI.EndChangeCheck() && needClose)
        {
            Close();
        }
    }

    void DoWindow(int id)
    {
        EditorGUI.BeginChangeCheck();
        EditorGUILayout.PrefixLabel("DNA名称:");
        DNAName = EditorGUILayout.TextField(DNAName);
        if (EditorGUI.EndChangeCheck())
        {
            nameExist = CheckPrefabIsExist(DNAName);
        }

        EditorGUILayout.BeginHorizontal();

        GUI.enabled = !nameExist;
        if (GUILayout.Button("创建"))
        {
            CreatePrefab(DNAName);
            needClose = true;
        }
        GUI.enabled = true;

        if (GUILayout.Button("取消"))
        {
            needClose = true;
        }
        EditorGUILayout.EndHorizontal();
    }
}