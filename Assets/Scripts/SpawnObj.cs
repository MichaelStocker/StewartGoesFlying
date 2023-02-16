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
    int successfulSpawns = 0;
    int failedSpawns = 0;
    public LayerMask poiLayer;

    // Start is called before the first frame update
    void Start()
    {
        while (successfulSpawns < controller.maxEggs)
        {
            Vector3 testLoc = Vector3.zero;
            testLoc.x += Random.Range(50, 9950);
            testLoc.z += Random.Range(50, 9950);
            testLoc.y = initialSpawnHeight;

            RaycastHit hit;
            if (Physics.Raycast(testLoc, Vector3.down, out hit, 1000, controller.terrainLayer))
            {
                float dist = (testLoc - hit.point).magnitude;
                print("Dist: " + dist);
                Collider[] poiLocs = Physics.OverlapSphere(hit.point, 800, poiLayer);
                if (dist > 460 && dist < 570 && poiLocs.Length == 0)
                {
                    GameObject temp = Instantiate(oasisObj, hit.point, Quaternion.identity);
                    successfulSpawns++;
                }
                else
                {
                    failedSpawns++;
                }
            }
            if (failedSpawns > 1000) break;
        }
        print("Fail :" + failedSpawns);
    }
}
