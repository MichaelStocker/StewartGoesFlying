using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class DookieAudio : MonoBehaviour
{
    public AudioClip dookieClip;
    PlaneController planeController;
    bool isActive = true;
    float timeNow = 0;
    float delay = 5;

    void Update()
    {
       if (timeNow < delay)
        {
            timeNow += Time.deltaTime;
        }
       else
        {
            timeNow = 0;
            isActive = true;
        }
    }
        private void OnTriggerEnter(Collider other)
    {
        if (other.transform.root.tag == "Player" && isActive)
        {
            isActive = false;
            AudioSource audioSource = planeController.secAud;
            audioSource.volume = 1;
            audioSource.PlayOneShot(dookieClip);
            timeNow = Time.deltaTime;
        }
    }
}
