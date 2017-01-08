using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Gene
{
    public Gene(DNA dna, int num = kDenom)
    {
        DNA = dna;
        Num = num;
    }

    public Gene Clone()
    {
        return new Gene(DNA, Num);
    }

    public void Combine(Gene other)
    {
        Num += other.Num;
    }

    public void MutationOne()
    {
        Num++;
    }

    public DNA DNA = null;

    int Num;

    const int kDenom = 100;

    public float Percent { get { return Num * 1.0f / kDenom; } }

    public int PercentI { get { return Num; } }

    public bool SetPercent(float v, ref float frac)
    {
        float realNumerator = v * kDenom;
        if (realNumerator < 1)
        {
            frac += realNumerator;
            Num = 0;
        }
        else
        {
            Num = Mathf.FloorToInt(realNumerator);
            frac += realNumerator - Num;
        }

        if (frac > 1.0f)
        {
            Num += 1;
            frac -= 1.0f;
        }

        return Num != 0;
    }

    public int Intensity
    {
        get { return Num * DNA.Intensity; }
    }
}

public class Genome
{
    Genome() { }

    static public Genome Breeding(Genome father, Genome mother)
    {
        Genome child = new Genome();
        List<Gene> childGenes = child.genes;

        //inherit genes from mother and father.
        foreach (Gene fatherGene in father.genes)
        {
            Gene newGene = fatherGene.Clone();
            childGenes.Add(newGene);
        }

        foreach (Gene motherGene in mother.genes)
        {
            Gene childGene = childGenes.Find(gene => gene.DNA == motherGene.DNA);
            if (childGene == null)
            {
                Gene newGene = motherGene.Clone();
                childGenes.Add(newGene);
            }
            else
            {
                childGene.Combine(motherGene);
            }
        }

        //bigger in front.
        childGenes.Sort((Gene a, Gene b) =>
        {
            if (a.Intensity > b.Intensity) return -1;
            if (a.Intensity < b.Intensity) return 1;
            return 0;
        });

        //normalize.
        float percent, frac = 0;
        List<Gene> removedGene = new List<Gene>();
        for (int i = 0; i < childGenes.Count; i++)
        {
            Gene gene = childGenes[i];
            percent = gene.Percent * 0.5f;
            if (!gene.SetPercent(percent, ref frac))
            {
                removedGene.Add(gene);
            }
        }

        removedGene.ForEach(rgene => childGenes.Remove(rgene));

        //gene mutation.
        //it is not a good solution.
        if (frac > 0.5)
        {
            childGenes[0].MutationOne();
        }


        child.FindStrongestGene();
        return child;
    }

    List<Gene> genes = new List<Gene>();
    List<Gene> strongest = new List<Gene>();

    public Genome(DNA dna)
    {
        genes.Add(new Gene(dna));
        FindStrongestGene();
    }

    public List<Gene> Genes
    {
        get { return genes; }
    }

    public DNA StrongestDNA
    {
        get
        {
            return (strongest.Count == 1
                ? strongest[0]
                : strongest[Random.Range(0, strongest.Count - 1)])
                .DNA;
        }
    }

    public DNA VisibileDNA
    {
        get
        {
            if (genes.Count == 1)
                return genes[0].DNA;

            List<int> randomTable = new List<int>();
            int sum = 0;
            foreach (Gene gene in genes)
            {
                int intensity = gene.Intensity;
                sum += intensity;
                randomTable.Add(sum);
            }

            int seed = Random.Range(0, sum + 1);
            for (int i = 0; i < randomTable.Count; i++)
            {
                if (seed < randomTable[i])
                    return genes[i].DNA;
            }
            //should not be here.
            return StrongestDNA;
        }
    }

    void FindStrongestGene()
    {
        int strongestNum = genes[0].Intensity;
        int i = 0;
        while (i < genes.Count && genes[i].Intensity == strongestNum)
        {
            strongest.Add(genes[i]);
            i++;
        }
    }
}
