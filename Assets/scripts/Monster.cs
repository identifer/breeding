using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Monster : MonoBehaviour
{
    Dictionary<string, float> propertyBag = new Dictionary<string, float>();

    float GetPropertyValue(string name, float defaultValue)
    {
        if (propertyBag.ContainsKey(name))
        {
            return propertyBag[name];
        }
        else
        {
            propertyBag[name] = defaultValue;
            return defaultValue;
        }
    }

    void SetPropertyValue(string name, float value)
    {
        propertyBag[name] = value;
    }

    //temp
    public Creature Creature = null;
    public Genome Genome = null;
    public Dictionary<string, float> Properties { get { return propertyBag; } }
    public void BuildProperty()
    {
        float DNALife = 0.0f;
        float DNAMana = 0.0f;
        float DNAPhysicalAttack = 0.0f;
        float DNAMagicalAttack = 0.0f;
        float DNAPhysicalDefense = 0.0f;
        float DNAMagicalDefense = 0.0f;
        float DNASpeed = 0.0f;
        foreach (Gene gene in Genome.Genes)
        {
            DNA dna = gene.DNA;
            int percent = gene.PercentI;
            DNALife += dna.Life * percent;
            DNAMana += dna.Mana * percent;
            DNAPhysicalAttack += dna.PhysicalAttack * percent;
            DNAMagicalAttack += dna.MagicalAttack * percent;
            DNAPhysicalDefense += dna.PhysicalDefense * percent;
            DNAMagicalDefense += dna.MagicalDefense * percent;
            DNASpeed += dna.Speed * percent;
        }

        const float kScale = 0.001f;// 1/1000

        CurrentLife = MaxLife = Creature.Life * DNALife * kScale;
        CurrentMana = MaxMana = Creature.Mana * DNAMana * kScale;
        PhysicalAttack = Creature.PhysicalAttack * DNAPhysicalAttack * kScale;
        MagicalAttack = Creature.MagicalAttack * DNAMagicalAttack * kScale;
        PhysicalDefense = Creature.PhysicalDefense * DNAPhysicalDefense * kScale;
        MagicalDefense = Creature.MagicalDefense * DNAMagicalDefense * kScale;
        Speed = Creature.Speed * DNASpeed * kScale;
        CriticalRate = Creature.CriticalRate * kScale;
        CriticalDamage = Creature.CriticalDamage * kScale;
    }

    public float MaxLife
    {
        get { return GetPropertyValue("MaxLife", 1); }
        set { SetPropertyValue("MaxLife", value); }
    }

    public float MaxMana
    {
        get { return GetPropertyValue("MaxMana", 1); }
        set { SetPropertyValue("MaxMana", value); }
    }

    public float CurrentLife
    {
        get { return GetPropertyValue("CurrentLife", MaxLife); }
        set { SetPropertyValue("CurrentLife", value); }
    }

    public float CurrentMana
    {
        get { return GetPropertyValue("CurrentMana", MaxLife); }
        set { SetPropertyValue("CurrentMana", value); }
    }

    public float PhysicalAttack
    {
        get { return GetPropertyValue("PhysicalAttack", 0); }
        set { SetPropertyValue("PhysicalAttack", value); }
    }

    public float MagicalAttack
    {
        get { return GetPropertyValue("MagicalAttack", 0); }
        set { SetPropertyValue("MagicalAttack", value); }
    }

    public float PhysicalDefense
    {
        get { return GetPropertyValue("PhysicalDefense", 0); }
        set { SetPropertyValue("PhysicalDefense", value); }
    }

    public float MagicalDefense
    {
        get { return GetPropertyValue("MagicalDefense", 0); }
        set { SetPropertyValue("MagicalDefense", value); }
    }

    public float Speed
    {
        get { return GetPropertyValue("Speed", 1); }
        set { SetPropertyValue("Speed", value); }
    }

    public float CriticalRate
    {
        get { return GetPropertyValue("CriticalRate", 0); }
        set { SetPropertyValue("CriticalRate", value); }
    }

    public float CriticalDamage
    {
        get { return GetPropertyValue("CriticalDamage", 150); }
        set { SetPropertyValue("CriticalDamage", value); }
    }
}
