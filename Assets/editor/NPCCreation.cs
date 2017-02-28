using UnityEditor;
using UnityEngine;

public class NPC
{
    [MenuItem("繁殖/创建NPC")]
    static void CreateNPCInWorld()
    {
        GameObject prefab = Resources.Load<GameObject>("World/NPCTemplate");
        if (prefab != null)
        {
            GameObject newNPC = GameObject.Instantiate(prefab);
            newNPC.GetComponentInChildren<MeshRenderer>().material = new Material(newNPC.GetComponentInChildren<MeshRenderer>().sharedMaterial);
            GameObject list = GameObject.Find("NPCs");
            if (list != null)
            {
                newNPC.transform.parent = list.transform;
            }
            newNPC.name = "NPC";
            Selection.activeGameObject = newNPC;
        }
    }
}
