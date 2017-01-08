using UnityEngine;

[System.Serializable]
public class DNA : MonoBehaviour
{
    public const string PrefabDirectory = "Assets/resources/DNA/";

    [SerializeField]
    public string Name;

    [SerializeField, Rename("DNA强度", 1, 100)]
    public int Intensity = 50;

    [SerializeField, Header("DNA 提供的属性"), Rename("生命值加成", 1, 50)]
    public int Life = 10;

    [SerializeField, Rename("法力值加成", 1, 50)]
    public int Mana = 10;

    [SerializeField, Rename("物理攻击力加成", 0, 20)]
    public int PhysicalAttack = 5;

    [SerializeField, Rename("法术攻击力加成", 0, 20)]
    public int MagicalAttack = 5;

    [SerializeField, Rename("物理防御力加成", 0, 20)]
    public int PhysicalDefense = 5;

    [SerializeField, Rename("法术防御力加成", 0, 20)]
    public int MagicalDefense = 5;

    [SerializeField, Rename("速度加成", 0, 20)]
    public int Speed = 5;

    public float RMGene { get { return Intensity * 0.01f; } }

    public float RMSpeed { get { return Speed * 0.05f; } }

    public float RMLife { get { return Life * 0.02f; } }

    public float RMMana { get { return Mana * 0.02f; } }

    public float RMPhysicOffense { get { return PhysicalAttack * 0.05f; } }

    public float RMPhysicDefense { get { return PhysicalDefense * 0.05f; } }

    public float RMMagicOffense { get { return MagicalAttack * 0.05f; } }

    public float RMMagicDefense { get { return MagicalDefense * 0.05f; } }
}
