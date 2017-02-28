using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using ConvAPI;

public class ConversationUI : MonoBehaviour
{
    private Conversation currentConversation;
    private Question currentQuestion;
    private NPC currentNPC;
    private List<AnswerButton> answerButtons = new List<AnswerButton>();
    private Queue<GameObject> reserveButtons = new Queue<GameObject>();
    private bool isShown = false;

    [SerializeField]
    GameObject dialogUI = null;

    [SerializeField]
    Text npcName = null;

    [SerializeField]
    Text questionTextUI = null;

    [SerializeField]
    Transform ButtonList = null;

    [SerializeField]
    GameObject ButtonPrefab = null;

    public int AnswerCount { get { return currentQuestion.AnswerCount; } }

    public int ButtonCount { get { return answerButtons.Count; } }

    public bool IsShown { get { return isShown; } }

    public void StartConversation(NPC who)
    {
        currentNPC = who;
        currentConversation = who.Conversation;
        World.Instance.Context.NPCSave = who.Save;
        currentQuestion = currentConversation.StartConversation(World.Instance.Context);
        
        dialogUI.SetActive(true);        
        isShown = true;
        RebuildUI();
    }

    public void SelectNextConversationBranch(int index)
    {
        Answer answer = currentQuestion.GetAnswerByIndex(index);
        if (answer != null)
        {
            Question question = currentConversation.SelectNextConversationBranch(answer);
            if (question != null)
            {
                currentQuestion = question;
                if (currentQuestion != null)
                {
                    RebuildUI();
                    isShown = true;
                    return;
                }
            }
        }
        
        dialogUI.SetActive(false);
        isShown = false;
    }

    public void PickAnswerByLabelIndex(int index)
    {
        if (index >= 0 && index < ButtonCount)
        {
            answerButtons[index].OnClick();
        }
    }

    void RebuildUI()
    {
        npcName.text = currentNPC.Name;
        questionTextUI.text = currentQuestion.Text.Replace("\\n", "\n");
        RemoveAnswerButtons();


        int labelIndex = 1;
        for (int i = 0, e = AnswerCount; i < e; i++)
        {
            Answer answer = currentQuestion.GetAnswerByIndex(i);

            if (answer == null || !answer.IsAvailable)
            {
                continue;
            }

            CreateAnswerButton(i, labelIndex, answer.Text);
            labelIndex++;
        }

        if (labelIndex == 1)
        {
            CreateAnswerButton(-1, labelIndex, "默默地离开……");
        }
    }

    void RemoveAnswerButtons()
    {
        List<Transform> removedButtons = new List<Transform>();
        foreach (Transform child in ButtonList)
        {
            removedButtons.Add(child);
        }

        removedButtons.ForEach((Transform button) =>
        {
            button.SetParent(null, false);
            reserveButtons.Enqueue(button.gameObject);
        });
        answerButtons.Clear();
    }

    AnswerButton CreateAnswerButton(int answerIndex, int labelIndex, string label)
    {
        GameObject btnObject = null;
        if (reserveButtons.Count > 0)
        {
            btnObject = reserveButtons.Dequeue();
            btnObject.transform.SetParent(ButtonList, false);
        }
        else
        {
            btnObject = Instantiate(ButtonPrefab, ButtonList);
        }

        AnswerButton answerButton = btnObject.GetComponent<AnswerButton>();
        answerButton.AnswerIndex = answerIndex;
        answerButton.LabelIndex = labelIndex;
        answerButton.Text = label;
        answerButton.Parent = this;
        answerButtons.Add(answerButton);
        return answerButton;
    }
}
