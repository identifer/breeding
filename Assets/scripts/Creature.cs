using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Creature : MonoBehaviour
{
    public const string PrefabDirectory = "Assets/resources/creature/";

    [SerializeField]
    public string Name = "";

    [SerializeField]
    public Sprite Sprite = null;

    [Header("怪物属性百分比")]
    [SerializeField, Rename("生命值", 1, 200)]
    public int Life = 30;

    [SerializeField, Rename("法力值", 1, 200)]
    public int Mana = 30;

    [SerializeField, Rename("物理攻击力", 1, 200)]
    public int PhysicalAttack = 30;

    [SerializeField, Rename("法术攻击力", 1, 200)]
    public int MagicalAttack = 30;

    [SerializeField, Rename("物理防御力", 1, 200)]
    public int PhysicalDefense = 30;

    [SerializeField, Rename("法术防御力", 1, 200)]
    public int MagicalDefense = 30;

    [SerializeField, Rename("速度", 1, 200)]
    public int Speed = 30;

    [SerializeField, Rename("暴击几率", 0, 100)]
    public int CriticalRate = 2;

    [SerializeField, Rename("暴击比例", 1, 1000)]
    public int CriticalDamage = 150;
}
