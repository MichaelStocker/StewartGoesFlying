using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Pickup : MonoBehaviour
{

    public void OnTriggerEnter(Collider other)
    {
        if (other.transform.root.tag == "Player")
        {
            PlaneController  planeController = other.transform.root.GetComponent<PlaneController>();
            Destroy(this.gameObject);
            planeController.eggsCollected++;
        }
    }






}
