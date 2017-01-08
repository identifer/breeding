using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Rename : PropertyAttribute
{
    string newName;
    bool useSlider;
    float minValue;
    float maxValue;

    public Rename(string name)
    {
        newName = name;
    }

    public Rename(string name, float min, float max)
    {
        newName = name;
        useSlider = true;
        minValue = min;
        maxValue = max;
    }

    public string NewName { get { return newName; } }
    public float MinValue { get { return minValue; } }
    public float MaxValue { get { return maxValue; } }
    public bool Range { get { return useSlider; } }
}