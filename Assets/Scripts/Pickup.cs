using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Pickup : MonoBehaviour
{
    public GameObject pickupParticle;
    bool isActive = true;
    public void OnTriggerEnter(Collider other)
    {
        if (other.transform.root.tag == "Player" && isActive)
        {
            isActive = false;
            print(other.gameObject.name);
            PlaneController  planeController = other.transform.root.GetComponent<PlaneController>();
            Instantiate(pickupParticle, transform.position, transform.rotation);
            planeController.eggsCollected++;
            Destroy(this.gameObject);
        }
    }






}
