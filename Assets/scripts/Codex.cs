using UnityEngine;
using System.Collections.Generic;

public class CodexEditor : PropertyAttribute { }

[System.Serializable]
public class CodexEntry
{
    [SerializeField]
    public Creature Creature = null;

    [SerializeField]
    public DNA FatherDNA = null;

    [SerializeField]
    public DNA MotherDNA = null;
}

public class Codex : MonoBehaviour
{
    public const string PrefabPath = "Assets/resources/codex.prefab";

    [CodexEditor]
    public string DNAPrefabPath = DNA.PrefabDirectory;

    [HideInInspector]
    public string CreaturePrefabPath = Creature.PrefabDirectory;

    [SerializeField]
    public List<DNA> DNAPool = new List<DNA>();

    [SerializeField]
    public List<Creature> CreaturePool = new List<Creature>();

    [SerializeField]
    public List<CodexEntry> CodexEntries = new List<CodexEntry>();
}
