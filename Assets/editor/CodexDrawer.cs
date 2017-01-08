using UnityEngine;
using UnityEditor;
using System.Collections.Generic;

[CustomPropertyDrawer(typeof(CodexEntry))]
public class CodexDrawer : PropertyDrawer
{
    static Codex Codex(SerializedProperty property)
    {
        return property.serializedObject.targetObject as Codex;
    }

    public override float GetPropertyHeight(SerializedProperty property, GUIContent label)
    {
        return 56;
    }

    string GetString(SerializedProperty property, string varName)
    {
        Object var = property.FindPropertyRelative(varName).objectReferenceValue;
        return (var == null) ? "null" : var.name;
    }

    public static string CreatureName(Creature creature) { return creature.name; }


    public override void OnGUI(Rect position, SerializedProperty property, GUIContent label)
    {
        EditorGUI.BeginProperty(position, label, property);
        Rect elementPos = position;

        elementPos.y += 4;
        elementPos.height = 16;
        EditorGUI.LabelField(elementPos, property.displayName, EditorStyles.boldLabel);

        //show pop ups.
        elementPos.y += 16;
        List<Creature> creaturePool = Codex(property).CreaturePool;
        Creature creature = property.FindPropertyRelative("Creature").objectReferenceValue as Creature;
        int selectIndex = creaturePool.IndexOf(creature);
        EditorGUI.BeginChangeCheck();
        selectIndex = EditorGUI.Popup(elementPos, selectIndex, creaturePool.ConvertAll(CreatureName).ToArray());
        //EditorGUI.PropertyField(elementPos, property.FindPropertyRelative("Creature"));
        if (EditorGUI.EndChangeCheck())
        {
            property.FindPropertyRelative("Creature").objectReferenceValue = creaturePool[selectIndex];
            property.serializedObject.ApplyModifiedProperties();
        }

        elementPos.y += 16;
        elementPos.width = position.width / 2;
        EditorGUI.LabelField(elementPos, "[父]:" + GetString(property, "FatherDNA"));
        elementPos.x += elementPos.width;


        EditorGUI.LabelField(elementPos, "[母]:" + GetString(property, "MotherDNA"));
        elementPos.x = position.x;
        elementPos.width = position.width;
        EditorGUI.EndProperty();
    }

}
