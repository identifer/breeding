using UnityEngine;
using UnityEditor;
using System.Collections.Generic;

[CustomPropertyDrawer(typeof(CodexEditor))]
public class CodexEditorDrawer : PropertyDrawer
{
    [MenuItem("繁殖/打开怪物图谱")]
    public static void OpenMonsterCodex()
    {
        GameObject prefab;
        if (!System.IO.File.Exists(global::Codex.PrefabPath))
        {
            prefab = CreateCodexPrefab(global::Codex.PrefabPath);
        }
        else
        {
            prefab = AssetDatabase.LoadAssetAtPath<GameObject>(global::Codex.PrefabPath);
        }
        Selection.activeGameObject = prefab;
    }

    static GameObject CreateCodexPrefab(string prefabPath)
    {
        GameObject codexGo = new GameObject("Codex");
        Codex codex = codexGo.AddComponent<Codex>();

        ReloadCreaturePool(codex);
        ReloadDNAPool(codex);
        UpdateCodex(codex);

        GameObject prefab = PrefabUtility.CreatePrefab(prefabPath, codexGo, ReplacePrefabOptions.Default);
        GameObject.DestroyImmediate(codexGo);
        return prefab;
    }

    Codex Codex(SerializedProperty property)
    {
        return property.serializedObject.targetObject as Codex;
    }

    public override float GetPropertyHeight(SerializedProperty property, GUIContent label)
    {
        return 20 * 5;
    }

    public override void OnGUI(Rect position, SerializedProperty property, GUIContent label)
    {
        Rect buttonRect = position;
        buttonRect.height = 16;
        GUI.Label(buttonRect, "DNA Pool:" + property.stringValue);

        buttonRect.y += 16;
        GUI.Label(buttonRect, "Creature Pool:" + Codex(property).CreaturePrefabPath);

        buttonRect.y += 20;
        buttonRect.height = 25;
        if (GUI.Button(buttonRect, "更新怪物图谱"))
        {
            ReloadDNAPool(Codex(property));
            UpdateCodex(Codex(property));
        }

        buttonRect.y += 25;
        if (GUI.Button(buttonRect, "刷新怪物列表"))
        {
            ReloadCreaturePool(Codex(property));
        }
    }

    static void ReloadCreaturePool(Codex codex)
    {
        string creaturePath = codex.CreaturePrefabPath;
        string[] fileList = null;
        if (System.IO.Directory.Exists(creaturePath))
        {
            fileList = System.IO.Directory.GetFiles(creaturePath, "*.prefab");
        }

        List<Creature> CreaturePool = codex.CreaturePool;

        //remove empty
        List<Creature> removedCreature = CreaturePool.FindAll(creature => creature == null);
        removedCreature.ForEach(creature => CreaturePool.Remove(creature));

        //insert new 
        if (fileList != null)
        {
            foreach (string fileName in fileList)
            {
                GameObject creatureGOPrefab = AssetDatabase.LoadAssetAtPath<GameObject>(fileName);
                if (creatureGOPrefab == null)
                    continue;

                Creature creaturePrefab = creatureGOPrefab.GetComponent<Creature>();
                if (creaturePrefab != null && !CreaturePool.Contains(creaturePrefab))
                {
                    CreaturePool.Add(creaturePrefab);
                }
            }
        }
    }

    static void ReloadDNAPool(Codex codex)
    {
        string DNAPath = codex.DNAPrefabPath;
        string[] fileList = null;
        if (System.IO.Directory.Exists(DNAPath))
        {
            fileList = System.IO.Directory.GetFiles(DNAPath, "*.prefab");
        }
        List<DNA> DNAPool = codex.DNAPool;

        //remove empty
        List<DNA> RemovedDNA = DNAPool.FindAll(dna => dna == null);
        RemovedDNA.ForEach(dna => DNAPool.Remove(dna));

        //insert new
        if (fileList != null)
        {
            foreach (string fileName in fileList)
            {
                GameObject dnaGOPrefab = AssetDatabase.LoadAssetAtPath<GameObject>(fileName);
                if (dnaGOPrefab == null)
                    continue;

                DNA dnaPrefab = dnaGOPrefab.GetComponent<DNA>();
                if (dnaPrefab != null && !DNAPool.Contains(dnaPrefab))
                {
                    DNAPool.Add(dnaPrefab);
                }
            }
        }
    }

    static void UpdateCodex(Codex codex)
    {
        List<DNA> DNAPool = codex.DNAPool;
        List<CodexEntry> Codex = codex.CodexEntries;

        //remove empty
        List<CodexEntry> RemovedCodex = Codex.FindAll(c => c.MotherDNA == null || c.FatherDNA == null);
        RemovedCodex.ForEach(c => Codex.Remove(c));

        for (int fatherID = 0; fatherID < DNAPool.Count; fatherID++)
        {
            for (int motherID = 0; motherID < DNAPool.Count; motherID++)
            {
                DNA fatherDNA = DNAPool[fatherID];
                DNA motherDNA = DNAPool[motherID];
                if (fatherDNA == null || motherDNA == null)
                    continue;

                CodexEntry c = Codex.Find(entry => entry.FatherDNA == fatherDNA && entry.MotherDNA == motherDNA);
                if (c != null)
                {
                    continue;
                }

                c = new CodexEntry();
                c.FatherDNA = fatherDNA;
                c.MotherDNA = motherDNA;
                Codex.Add(c);
            }
        }
    }
}
