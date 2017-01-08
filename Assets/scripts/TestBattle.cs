using System.Collections;
using System.Collections.Generic;
using UnityEngine;

class BattleUnit
{
    public Monster Monster;
    public bool IsAlien = false;
    public float NextSpeed = 0;
    public int Index = 0;

}
public class TestBattle : MonoBehaviour
{
    [SerializeField]
    Codex Codex;

    [SerializeField]
    GameObject SelectBound;

    GameObject[] enemy = new GameObject[9];
    GameObject[] alien = new GameObject[9];
    //LinkedList<BattleUnit> battleUnits = new LinkedList<BattleUnit>();
    List<BattleUnit> battleUnits = new List<BattleUnit>();
    float TurnComsume = 100;
    bool needPlus = false;

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

    void SpawnMonster(Monster monster)
    {
        Genome fatherGenome = new Genome(Codex.DNAPool[Random.Range(0, Codex.DNAPool.Count)]);
        Genome motherGenome = new Genome(Codex.DNAPool[Random.Range(0, Codex.DNAPool.Count)]);

        monster.Genome = Genome.Breeding(fatherGenome, motherGenome);
        monster.Creature = CreateCreature(fatherGenome.VisibileDNA, motherGenome.VisibileDNA);
        monster.BuildProperty();
    }

    void Start()
    {
        for (int i = 0; i < 3; i++)
        {
            GameObject monsterGO = new GameObject("enemy" + (i + 1));
            Monster monster = monsterGO.AddComponent<Monster>();
            SpriteRenderer render = monsterGO.AddComponent<SpriteRenderer>();

            SpawnMonster(monster);
            render.sprite = monster.Creature.Sprite;

            int col = Random.Range(0, 3);
            enemy[col + i * 3] = monsterGO;
            enemy[col + i * 3].transform.position = new Vector3(i * 1, col + 1, 0);

            BattleUnit unit = new BattleUnit();
            unit.Monster = monster;
            unit.Index = col + i * 3;
            unit.IsAlien = false;
            battleUnits.Add(unit);
        }

        for (int i = 0; i < 3; i++)
        {
            GameObject monsterGO = new GameObject("alien" + (i + 1));
            Monster monster = monsterGO.AddComponent<Monster>();
            SpriteRenderer render = monsterGO.AddComponent<SpriteRenderer>();

            SpawnMonster(monster);
            render.sprite = monster.Creature.Sprite;

            int col = Random.Range(0, 3);
            alien[col + i * 3] = monsterGO;
            alien[col + i * 3].transform.position = new Vector3(i * 1, -col - 1, 0);

            BattleUnit unit = new BattleUnit();
            unit.Monster = monster;
            unit.Index = col + i * 3;
            unit.IsAlien = true;
            battleUnits.Add(unit);
        }

        bool enough = false;
        while (!enough)
        {
            foreach (BattleUnit unit in battleUnits)
            {
                unit.NextSpeed += unit.Monster.Speed;
                enough = enough || (unit.NextSpeed > TurnComsume);
            }
        }
        battleUnits.Sort(SpeedSorter); SelectBound.transform.position = battleUnits[0].Monster.transform.position;
    }

    int SpeedSorter(BattleUnit a, BattleUnit b)
    {

        if (a.NextSpeed > b.NextSpeed)
            return -1;
        else if (a.NextSpeed < b.NextSpeed)
            return 1;
        else if (a.IsAlien != b.IsAlien)
            return (a.IsAlien && !b.IsAlien) ? -1 : 1;
        else
            return 0;
    }

    bool Next()
    {
        battleUnits[0].NextSpeed -= TurnComsume;
        bool result = true;
        bool enough = false;
        foreach (BattleUnit unit in battleUnits)
        {
            if (unit.NextSpeed > TurnComsume)
            {
                enough = true;
                result = false;
                break;
            }
        }
        while (!enough)
        {
            foreach (BattleUnit unit in battleUnits)
            {
                unit.NextSpeed += unit.Monster.Creature.Speed;
                enough = enough || (unit.NextSpeed > TurnComsume);
            }
        }
        battleUnits.Sort(SpeedSorter);
        SelectBound.transform.position = battleUnits[0].Monster.transform.position;
        return result;
    }

    // Update is called once per frame
    void OnGUI()
    {
        GUILayout.Label("敌人");
        GUILayout.BeginHorizontal();
        for (int i = 0; i < 9; i++)
        {
            if (enemy[i] == null)
                continue;
            Monster monster = enemy[i].GetComponent<Monster>();
            List<Gene> genes = monster.Genome.Genes;
            GUILayout.BeginVertical();
            GUILayout.Label(monster.Creature.Name);
            foreach (Gene gene in genes)
            {
                GUILayout.Label( gene.DNA.Name + ":" + gene.Percent * 100.0f + "%");
            }
            GUILayout.Label("速度：" + monster.Speed);
            GUILayout.EndVertical();
            GUILayout.Space(5);
        }
        GUILayout.EndHorizontal();
        GUILayout.Label("友军");
        GUILayout.BeginHorizontal();
        for (int i = 0; i < 9; i++)
        {
            if (alien[i] == null)
                continue;
            Monster monster = alien[i].GetComponent<Monster>();
            List<Gene> genes = monster.Genome.Genes;
            GUILayout.BeginVertical();
            GUILayout.Label(monster.Creature.Name);
            foreach (Gene gene in genes)
            {
                GUILayout.Label(gene.DNA.Name + ":" + gene.Percent * 100.0f + "%");
            }
            GUILayout.Label("速度：" + monster.Speed);
            GUILayout.EndVertical();
            GUILayout.Space(10);
        }
        GUILayout.EndHorizontal();
        GUILayout.Space(10);
        GUILayout.Label("行动顺序:");
        foreach (BattleUnit unit in battleUnits)
        {
            string prefix = ((unit.IsAlien) ? "友" : "敌")
                + "[" + unit.Index + "]:";
            GUILayout.BeginHorizontal();
            GUILayout.Label(prefix + unit.Monster.Creature.Name);
            if (needPlus)
            {
                GUILayout.Label("行动力：" + unit.NextSpeed + "=" + (unit.NextSpeed - unit.Monster.Speed) + "+" + unit.Monster.Speed);
            }
            else
            {
                GUILayout.Label("行动力：" + unit.NextSpeed);
            }
            GUILayout.EndHorizontal();
        }

        if (GUI.Button(new Rect(300, 20, 100, 20), "下一个"))
        {
            needPlus = Next();
        }
    }
}
