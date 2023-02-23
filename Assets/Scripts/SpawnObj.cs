using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class SpawnObj : MonoBehaviour
{
    public GameObject oasisObj;
    int initialSpawnHeight = 800;
    int terrainHeight = 600;
    int spawnHeightMax = 340;
    int spawnHeightMin = 230;
    public PlaneController controller;
    bool spawnComplete;

    // Start is called before the first frame update
    void Start()
    {
        while (!spawnComplete)
        {
            Vector3 testLoc = Vector3.zero;
            testLoc.x += transform.position.x + Random.Range(-250, 250);
            testLoc.z += transform.position.z + Random.Range(-250, 250);
            testLoc.y = initialSpawnHeight;

            RaycastHit hit;
            if (Physics.Raycast(testLoc, Vector3.down, out hit, 1000, controller.terrainLayer))
            {
                float dist = (testLoc - hit.point).magnitude;

                if (dist > 460 && dist < 570)
                {
                    GameObject temp = Instantiate(oasisObj, hit.point, Quaternion.identity);
                    controller.maxEggs++;
                    spawnComplete = true;
                    print(controller.maxEggs);
                }

            }
        }
    }
}
