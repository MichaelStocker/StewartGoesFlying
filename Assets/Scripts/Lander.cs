using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Lander : MonoBehaviour
{
    [SerializeField] Transform t1;
    [SerializeField] Transform t2;
    [SerializeField] Transform t3;

    [SerializeField] LayerMask terrainLayer;
    [SerializeField] float landingHeight = 5;

    void Start()
    {

    }

    void Update()
    {
        RaycastHit hitA;
        RaycastHit hitB;
        RaycastHit hitC;

        if (Physics.Raycast(t1.position, Vector3.down, out hitA, landingHeight, terrainLayer))
        {
            if (Physics.Raycast(t2.position, Vector3.down, out hitB, landingHeight, terrainLayer))
            {
                if (Physics.Raycast(t3.position, Vector3.down, out hitC, landingHeight, terrainLayer))
                {
                    Vector3 v1 = hitB.point - hitA.point;
                    Vector3 v2 = hitC.point - hitA.point;
                    Vector3 vc = Vector3.Cross(v1, v2);

                    float amount = 1 - Mathf.Clamp01((transform.position - hitC.point).magnitude / landingHeight);
                    print(amount);

                    Quaternion dq = Quaternion.FromToRotation(transform.up, vc);
                    transform.rotation = Quaternion.Slerp(transform.rotation, dq * transform.rotation, 0.002f);
                }
            }
        }

    }
}
