using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Pickup : MonoBehaviour
{
    [SerializeField] GameObject egg;
    [SerializeField] GameObject rock;


    public void OnTriggerEnter(Collider other)
    {
        Item temp = other.GetComponent<Item>();
        if (temp != null)
        {
            if (temp.itemType == Item.types.Rock)
            {
                // Rock Stuff
            }
            if (temp.itemType == Item.types.Egg)
            {
                // Egg Stuff
            }
        }
    }






}
