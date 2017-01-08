using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class TestUI : MonoBehaviour
{
    public GameObject RadarPrefab;

    Codex Codex = null;
    Monster monster = null;
    List<string> DNANames = new List<string>();

    void Start()
    {
        Random.InitState(0xCA51B7);
        Codex = Resources.Load<GameObject>("codex").GetComponent<Codex>();

        Codex.DNAPool.ForEach(dna => DNANames.Add(dna.name));
    }

    bool showFatherList = false;
    bool showMotherList = false;
    bool showOtherList = false;
    int fatherID = -1;
    int motherID = -1;
    int otherID = -1;

    Vector3 RadarMapPosF = new Vector3(-2, 1, 0);
    Vector3 RadarMapPosM = new Vector3(4, 1, 0);
    Vector3 RadarMapPosO = new Vector3(0, 4, 0);
    RadarMap fatherMap = null;
    RadarMap motherMap = null;
    RadarMap otherMap = null;


    void OnGUI()
    {
        if (monster == null)
        {
            DrawSpawnerUI();
        }
        else
        {
            DrawBreedingUI();
            DrawCurrentMonster();
        }
    }

    void CreateMonster()
    {
        if (monster != null)
        {
            GameObject.Destroy(monster.gameObject);
        }

        GameObject monsterGO = new GameObject("monster");
        monster = monsterGO.AddComponent<Monster>();
    }

    void SpawnMonster()
    {
        GameObject.Destroy(fatherMap.gameObject);
        GameObject.Destroy(motherMap.gameObject);

        Genome fatherGenome = new Genome(Codex.DNAPool[fatherID]);
        Genome motherGenome = new Genome(Codex.DNAPool[motherID]);

        CreateMonster();
        monster.Genome = Genome.Breeding(fatherGenome, motherGenome);
        monster.Creature = CreateCreature(fatherGenome.VisibileDNA, motherGenome.VisibileDNA);
        monster.BuildProperty();
    }

    void BreedingNextGeneration()
    {
        Genome oldGenome = monster.Genome;
        Genome otherGenome = new Genome(Codex.DNAPool[otherID]);
        Genome newGenome;
        Creature creature;
        if (Random.Range(0, 1) == 1)
        {
            creature = CreateCreature(oldGenome.VisibileDNA, otherGenome.VisibileDNA);
            newGenome = Genome.Breeding(oldGenome, otherGenome);

        }
        else
        {
            creature = CreateCreature(otherGenome.VisibileDNA, oldGenome.VisibileDNA);
            newGenome = Genome.Breeding(otherGenome, oldGenome);
        }

        CreateMonster();
        monster.Creature = creature;
        monster.Genome = newGenome;
        monster.BuildProperty();
    }

    Creature CreateCreature(DNA father, DNA mother)
    {
        CodexEntry entry = Codex.CodexEntries.Find(ce => ce.FatherDNA == father && ce.MotherDNA == mother);
        if (entry != null)
        {
            return entry.Creature;
        }
        else
        {
            Debug.Log("error");
            return null;
        }
    }

    void DrawSpawnerUI()
    {
        Rect uiRect = new Rect(0, 0, 100, 20);
        GUI.Label(uiRect, "创建新物种");

        uiRect.y += 20;
        fatherID = DrawList(uiRect, "选择父系DNA", fatherID, ref showFatherList);
        if (fatherID != -1)
        {
            if (fatherMap == null)
            {
                GameObject frmGO = GameObject.Instantiate<GameObject>(RadarPrefab);
                frmGO.transform.position = RadarMapPosF;
                fatherMap = frmGO.GetComponent<RadarMap>();
            }
            SetRadarMap(fatherMap, Codex.DNAPool[fatherID]);
        }

        uiRect.y += 20;
        motherID = DrawList(uiRect, "选择母系DNA", motherID, ref showMotherList);
        if (motherID != -1)
        {
            if (motherMap == null)
            {
                GameObject mrmGO = GameObject.Instantiate<GameObject>(RadarPrefab);
                mrmGO.transform.position = RadarMapPosM;
                motherMap = mrmGO.GetComponent<RadarMap>();
            }
            SetRadarMap(motherMap, Codex.DNAPool[motherID]);
        }

        if (fatherID != -1 && motherID != -1)
        {
            uiRect.y += 20;
            if (GUI.Button(uiRect, "繁殖!"))
            {
                SpawnMonster();
            }
        }
    }

    int DrawList(Rect uiRect, string defaultString, int ID, ref bool showList)
    {
        if (GUI.Button(uiRect, ID == -1 ? defaultString : DNANames[ID]))
        {
            showList = !showList;
        }

        if (showList)
        {
            Rect listUIRect = uiRect;
            listUIRect.x += listUIRect.width;
            listUIRect.width = 320;
            listUIRect.height = 30 * DNANames.Count / 3;

            GUI.changed = false;
            ID = GUI.SelectionGrid(listUIRect, ID, DNANames.ToArray(), 4);
            if (GUI.changed)
            {
                showList = false;
            }
        }
        return ID;
    }

    void DrawBreedingUI()
    {
        Rect uiRect = new Rect(0, 0, 100, 20);
        GUI.Label(uiRect, "繁殖后代");

        uiRect.y += 20;
        otherID = DrawList(uiRect, "选择配偶DNA", otherID, ref showOtherList);
        if (otherID != -1)
        {
            if (otherMap == null)
            {
                GameObject ormGO = GameObject.Instantiate<GameObject>(RadarPrefab);
                ormGO.transform.position = RadarMapPosO;
                otherMap = ormGO.GetComponent<RadarMap>();
            }
            SetRadarMap(otherMap, Codex.DNAPool[otherID]);
            uiRect.y += 20;
            if (GUI.Button(uiRect, "繁殖!"))
            {
                BreedingNextGeneration();
            }
        }
    }

    void DrawCurrentMonster()
    {
        Rect uiRect = new Rect(150, 150, 400, 100);
        List<Gene> genes = monster.Genome.Genes;

        GUI.Label(uiRect, "当前怪物：" + monster.Creature.Name);
        foreach (Gene gene in genes)
        {
            uiRect.y += 20;
            GUI.Label(uiRect, gene.DNA.Name + ":" + gene.Percent * 100.0f + "%");
        }

        uiRect.y += 30;
        GUI.Label(uiRect, "当前属性：");
        foreach (KeyValuePair<string, float> p in monster.Properties)
        {
            uiRect.y += 20;
            GUI.Label(uiRect, p.Key + ":" + p.Value);
        }
    }

    void SetRadarMap(RadarMap map, DNA dna)
    {
        map.ResetMap(dna);
    }
}
