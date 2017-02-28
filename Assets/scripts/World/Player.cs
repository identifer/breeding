using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Player : MonoBehaviour
{
    const float extends = 0.2f;
    SphereCollider trigger = null;
    Vector3 colliderOffset;
    float colliderRadius = 0;

    public void Face(Vector3 direction)
    {
        float x = direction.x * colliderRadius;
        float z = direction.z * colliderRadius;

        trigger.center = colliderOffset + new Vector3(x, 0, z);
    }

    private void Awake()
    {
        CapsuleCollider collider = GetComponent<CapsuleCollider>();
        trigger = GetComponent<SphereCollider>();
        colliderOffset = collider.center;
        colliderRadius = collider.radius;
    }
    
    private void OnTriggerEnter(Collider collider)
    {
        NPC npc = collider.GetComponent<NPC>();
        if (npc != null)
        {
            World.Instance.SetHoverNPC(npc);
        }
        else
        {
            World.Instance.SetHoverNPC(null);
        }
    }

    private void OnTriggerExit(Collider collider)
    {
        World.Instance.SetHoverNPC(null);
    }

}
