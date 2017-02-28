using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[RequireComponent(typeof(Rigidbody))]
public class Charactor : MonoBehaviour
{
    private Rigidbody body = null;

    private void Awake()
    {
        body = GetComponent<Rigidbody>();
    }

    public void MoveTo(Vector3 position)
    {
        body.MovePosition(position);
    }

    public void Move(Vector3 offset)
    {
        MoveTo(body.position + offset);
    }

    public void Facing(Vector3 direction)
    {
        transform.right = direction;
    }

    public Vector3 position
    {
        get { return body.position; }
    }
}
