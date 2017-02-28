using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using ConvAPI;
using System.IO;

[RequireComponent(typeof(Charactor))]
[SelectionBase]
public class NPC : MonoBehaviour
{
    static void ShowNPCName(int npcID)
    {
        if (NPCs.ContainsKey(npcID))
        {
            NPCs[npcID].save.Set("show_name", true, true);
        }
    }

    public static Dictionary<int, NPC> NPCs = new Dictionary<int, NPC>();

    //Charactor charactor = null;

    [SerializeField]
    int id = 0;

    [SerializeField]
    string descName = "";

    [SerializeField]
    string realName = "";

    [SerializeField]
    string conversationFileName = "";

    Save save = null;

    Conversation conversation;

    public Save Save { get { return save; } }

    public int ID { get { return id; } }

    public Conversation Conversation { get { return conversation; } }

    public string Name
    {
        get
        {
            Value sv = save.GetValue("show_name");
            if (sv != null && sv.Boolean)
            {
                return realName;
            }
            else
            {
                return descName;
            }
        }
    }

    private void Awake()
    {
        if (ID == 0 || NPCs.ContainsKey(ID))
        {
            Destroy(gameObject);
            return;
        }

        conversation = Conversation.CreateFromXML(Application.streamingAssetsPath + "/" + conversationFileName);
        if (conversation == null)
        {
            Destroy(gameObject);
            return;
        }

        NPCs.Add(ID, this);
        //charactor = GetComponent<Charactor>();

        InitializeSave();
    }

    private void OnDestroy()
    {
        if (save != null)
        {
            string saveName = Application.streamingAssetsPath + "/" + ID + ".save";
            save.SaveToFile(saveName);
            save.Dispose();
        }
    }

    /*
    private void OnTriggerStay2D(Collider2D other)
    {
        if (!other.isTrigger)
        {
            if (other.GetComponent<Player>() != null)
            {
                Vector3 targetPos = new Vector2(other.transform.position.x,
                                        other.transform.position.y);
                Vector3 dir = targetPos - charactor.position;
                charactor.Facing(dir.normalized);
            }
        }
    }
    */
    private void InitializeSave()
    {
        save = new Save();
        string saveName = Application.streamingAssetsPath + "/" + ID + ".save";
        if (File.Exists(saveName))
        {
            save.LoadFromFile(saveName);
        }

        save.Set("id", ID, true);
        Value v = save.GetValue("show_name");
        if (v != null)
        {
            v.ReadOnly = true;
        }
        else
        {
            save.Set("show_name", false, true);
        }
    }
}
