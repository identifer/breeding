using System.Collections.Generic;
using UnityEngine;
using UnityEditor;
using System.Linq;

[CustomPropertyDrawer(typeof(Rename))]
class RenameDrawer : PropertyDrawer
{
    bool showChildren = true;
    Rename Rename { get { return attribute as Rename; } }

    public override float GetPropertyHeight(SerializedProperty property, GUIContent label)
    {
        return showChildren ? EditorGUI.GetPropertyHeight(property, label) : 16;
    }

    public override void OnGUI(Rect position, SerializedProperty property, GUIContent label)
    {
        label.text = Rename.NewName;
        object[] attributes = null;

        if (Rename.Range)
        {
            DrawSlider(position, property, label, Rename.MinValue, Rename.MaxValue);
        }
        else if ((attributes = fieldInfo.GetCustomAttributes(typeof(RangeAttribute), true)) != null && attributes.Length > 0)
        {
            RangeAttribute range = attributes[0] as RangeAttribute;
            DrawSlider(position, property, label, range.min, range.max);
        }
        else
        {
            showChildren = EditorGUI.PropertyField(position, property, label, showChildren);
        }
    }

    void DrawSlider(Rect position, SerializedProperty property, GUIContent label, float min, float max)
    {
        if (property.propertyType == SerializedPropertyType.Float)
            EditorGUI.Slider(position, property, min, max, label);
        else if (property.propertyType == SerializedPropertyType.Integer)
            EditorGUI.IntSlider(position, property, (int)min, (int)max, label);
        else
            EditorGUI.LabelField(position, label.text, "Use Range with float or int.");
    }
}

public static class EditorEx
{
    static Object[] windows = null;
    static System.Reflection.FieldInfo showModeField = null;
    static System.Reflection.PropertyInfo positionProperty = null;

    static System.Type[] GetAllDerivedTypes(this System.AppDomain aAppDomain, System.Type aType)
    {
        List<System.Type> result = new List<System.Type>();
        var assemblies = aAppDomain.GetAssemblies();
        foreach (var assembly in assemblies)
        {
            var types = assembly.GetTypes();
            foreach (var type in types)
            {
                if (type.IsSubclassOf(aType))
                    result.Add(type);
            }
        }
        return result.ToArray();
    }

    static void EnumWindows()
    {
        var containerWinType = System.AppDomain.CurrentDomain.GetAllDerivedTypes(typeof(ScriptableObject)).Where(t => t.Name == "ContainerWindow").FirstOrDefault();
        if (containerWinType == null)
            throw new System.MissingMemberException("Can't find internal type ContainerWindow. Maybe something has changed inside Unity");

        showModeField = containerWinType.GetField("m_ShowMode", System.Reflection.BindingFlags.NonPublic | System.Reflection.BindingFlags.Instance);
        positionProperty = containerWinType.GetProperty("position", System.Reflection.BindingFlags.Public | System.Reflection.BindingFlags.Instance);
        if (showModeField == null || positionProperty == null)
            throw new System.MissingFieldException("Can't find internal fields 'm_ShowMode' or 'position'. Maybe something has changed inside Unity");

        windows = Resources.FindObjectsOfTypeAll(containerWinType);
    }

    static Rect GetEditorMainWindowPos()
    {
        if (windows == null || showModeField == null || positionProperty == null)
        {
            EnumWindows();
        }

        foreach (var win in windows)
        {
            var showmode = (int)showModeField.GetValue(win);
            if (showmode == 4) // main window
            {
                var pos = (Rect)positionProperty.GetValue(win, null);
                return pos;
            }
        }
        throw new System.NotSupportedException("Can't find internal main window. Maybe something has changed inside Unity");
    }

    public static void CenterOnMainWin(this UnityEditor.EditorWindow aWin)
    {
        var main = GetEditorMainWindowPos();
        var pos = aWin.position;
        float w = (main.width - pos.width) * 0.5f;
        float h = (main.height - pos.height) * 0.5f;
        pos.x = main.x + w;
        pos.y = main.y + h;
        aWin.position = pos;
    }
}