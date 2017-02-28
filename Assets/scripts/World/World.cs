using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using ConvAPI;
using UnityEngine.UI;
using System.IO;

public class World : MonoBehaviour
{
    static World instance = null;

    public Context Context = null;

    [SerializeField]
    Charactor charactor = null;
    Player player = null;

    [SerializeField]
    Transform mainCamera = null;

    [SerializeField]
    float movingSpeed = 1.0f;

    [SerializeField]
    ConversationUI convUI = null;

    NPC hoverNPC = null;
    bool inConversation = false;

    void Awake()
    {
        instance = this;
        Context = new Context();
        FunctionAdapter adapter = FunctionAdapter.Create(typeof(NPC), "ShowNPCName");
        Context.Register(adapter, "show_npc_name");

        //加载数据
        string path = Application.streamingAssetsPath + "/";
        if (File.Exists(path + "global.save"))
            Context.GlobalSave.LoadFromFile(path + "global.save");

        if (File.Exists(path + "player.save"))
            Context.PlayerSave.LoadFromFile(path + "player.save");

        player = charactor.GetComponent<Player>();
    }

    private void OnDestroy()
    {
        //加载数据
        string path = Application.streamingAssetsPath + "/";
        Context.GlobalSave.SaveToFile(path + "global.save");
        Context.PlayerSave.SaveToFile(path + "player.save");
    }

    void FixedUpdate()
    {
        if (convUI.IsShown)
        {
            ConversationInput();
        }
        else
        {
            MovementUpdate();

            mainCamera.position = new Vector3(
                charactor.position.x,
                mainCamera.position.y,
                charactor.position.z - 4);
        }
    }

    void ConversationInput()
    {
        for (int i = 0, e = convUI.ButtonCount; i < e; i++)
        {
            if (Input.GetKeyUp(KeyCode.Alpha1 + i))
            {
                convUI.PickAnswerByLabelIndex(i);
            }
        }
    }

    void MovementUpdate()
    {
        if (Input.GetButton("Horizontal") || Input.GetButton("Vertical"))
        {
            Vector3 direction = new Vector3(Input.GetAxis("Horizontal"), 0, Input.GetAxis("Vertical")).normalized;

            Vector3 offset = direction * Time.fixedDeltaTime * movingSpeed;
            charactor.Move(offset);
            player.Face(direction);
        }

        if (Input.GetButton("Interact"))
        {
            if (HoverNPC != null)
            {
                convUI.StartConversation(HoverNPC);
                inConversation = true;
            }
        }

    }

    public void SetHoverNPC(NPC npc)
    {
        hoverNPC = npc;
    }
    public static World Instance { get { return instance; } }

    public NPC HoverNPC { get { return hoverNPC; } }

    public bool IsInConversation { get { return inConversation; } }
}
