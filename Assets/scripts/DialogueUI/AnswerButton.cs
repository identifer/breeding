using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class AnswerButton : MonoBehaviour
{
    public int LabelIndex = 0;

    public int AnswerIndex = 0;

    public ConversationUI Parent = null;

    string label = "";
    Text textComponent = null;
    Button buttonComponent = null;

    public string Text
    {
        set
        {
            label = LabelIndex.ToString() + "." + value;
            textComponent.text = label;
        }
    }

    public void OnClick()
    {
        Parent.SelectNextConversationBranch(AnswerIndex);
    }

    void Awake()
    {
        textComponent = GetComponentInChildren<Text>();
        buttonComponent = GetComponent<Button>();
        buttonComponent.onClick.AddListener(OnClick);
    }

}
