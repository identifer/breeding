using UnityEngine;
using UnityEditor;

public class CreatureCreationWindow : EditorWindow
{
    [MenuItem("繁殖/创建怪物")]
    public static void OpenCreatureCreationWindow()
    {
        //show prompt.
        CreatureCreationWindow window = ScriptableObject.CreateInstance<CreatureCreationWindow>();
        window.position = new Rect(0, 0, 200, 100);
        EditorEx.CenterOnMainWin(window);

        window.ShowPopup();
    }

    void CreatePrefab(string name)
    {
        //Create GameObject
        GameObject creatureGO = new GameObject(name);
        Creature creature = creatureGO.AddComponent<Creature>();
        creature.Name = name;
        Selection.activeGameObject = creatureGO;

        //make sure the directory is exist.
        if (!System.IO.Directory.Exists(Creature.PrefabDirectory))
        {
            System.IO.Directory.CreateDirectory(Creature.PrefabDirectory);
        }

        //create prefab.
        string PrefabPath = MakeFullPath(name);
        GameObject prefab = PrefabUtility.CreatePrefab(PrefabPath, creatureGO, ReplacePrefabOptions.Default);
        GameObject.DestroyImmediate(creatureGO);
        EditorGUIUtility.PingObject(prefab);
        Selection.activeGameObject = prefab;
    }

    string ValidateFileName(string name)
    {
        return name.Replace(' ', '_');
    }

    string MakeFullPath(string name)
    {
        return Creature.PrefabDirectory + ValidateFileName(name) + ".prefab";
    }

    bool CheckPrefabIsExist(string name)
    {
        return System.IO.File.Exists(MakeFullPath(name));
    }

    string creatureName = "New Creature";
    Rect windowRect = new Rect(1, 1, 199, 99);
    bool needClose = false;
    bool nameExist = false;

    CreatureCreationWindow()
    {
        nameExist = CheckPrefabIsExist(creatureName);
    }

    void OnGUI()
    {
        EditorGUI.BeginChangeCheck();
        BeginWindows();
        windowRect = GUI.Window(1, windowRect, DoWindow, "创建一个新的怪物");
        EndWindows();
        if (EditorGUI.EndChangeCheck() && needClose)
        {
            Close();
        }
    }

    void DoWindow(int id)
    {
        EditorGUI.BeginChangeCheck();
        EditorGUILayout.PrefixLabel("怪物名称:");
        creatureName = EditorGUILayout.TextField(creatureName);
        if (EditorGUI.EndChangeCheck())
        {
            nameExist = CheckPrefabIsExist(creatureName);
        }

        EditorGUILayout.BeginHorizontal();

        GUI.enabled = !nameExist;
        if (GUILayout.Button("创建"))
        {
            CreatePrefab(creatureName);
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