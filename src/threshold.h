// return the threshold value in ms for the given HR
// table 2 from 10.1109/MOCAST52088.2021.9493400
float threshold_from_HR(float HR)
{
  if (HR < 60)
  {
    return 400000;
  }
  else if (HR < 70)
  {
    return 360000;
  }
  else if (HR < 80)
  {
    return 320000;
  }
  else if (HR < 100)
  {
    return 280000;
  }
  else if (HR < 120)
  {
    return 250000;
  }
  else if (HR < 140)
  {
    return 220000;
  }
  else if (HR < 160)
  {
    return 200000;
  }
  else
  {
    return 180000;
  }
}