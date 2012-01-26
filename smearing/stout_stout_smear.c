#include "stout.ih"

int stout_smear(gauge_field_t m_field_out, struct stout_parameters const *params, gauge_field_t m_field_in)
{
  static su3 tmp;
  gauge_field_t buffer = get_gauge_field();
  
  /* start of the the stout smearing **/
  for(int iter = 0; iter < params->iterations; ++iter)
  {
    for (int x = 0; x < VOLUME; ++x)
      for (int mu = 0; mu < 4; ++mu)
      {
        generic_staples(&tmp, x, mu, m_field_in);
        _real_times_su3(tmp, params->rho, tmp);
        _su3_times_su3d(buffer.field[x][mu], tmp, m_field_in.field[x][mu]);
        project_antiherm(&buffer.field[x][mu]);
        exposu3_in_place(&buffer.field[x][mu]);
      }
    
    for(int x = 0; x < VOLUME; ++x)
      for(int mu = 0 ; mu < 4; ++mu)
      { 
        /* Input and output are allowed to be aliases -- use tmp */
        _su3_times_su3(tmp, buffer.field[x][mu], m_field_in.field[x][mu]);
        _su3_assign(m_field_out.field[x][mu], tmp);
      }

    generic_exchange(m_field_out.field, sizeof(su3_tuple));
    m_field_in.field = m_field_out.field; /* Prepare for next iteration */
  }
  
  return_gauge_field(&buffer);

  return(0);
}
