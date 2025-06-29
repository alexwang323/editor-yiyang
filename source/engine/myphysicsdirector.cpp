//#include "modules/collider/collider.h"
/*
 * this is the physicsdirectorclass
 * todos work on the colliders,
 * work on the integrator
 * work on reading the geodot thing
 */
#include "ginkgopch.h"
#include "math.h"
#include<cmath>
#include "myphysicsdirector.h"

constexpr auto TIMEDELTA = 1.0/60.0;
#define ITERATION_MAX_EPA 10

collider* Myphysicsdirector::player = nullptr;
inline bool dircheck(const glm::vec3& direction, const pointcombo& ao)
{
    return (glm::dot(direction,ao.sum)) > 0;
}
inline bool dircheck(const glm::vec3& direction, const glm::vec3& ao)
{
    return (glm::dot(direction,ao)) > 0;
}


collider* Myphysicsdirector::getPlayer()
{
    return nullptr;
}

void Myphysicsdirector::setPlayer(collider* p)
{
    Myphysicsdirector::player = p;
}

Myphysicsdirector::Myphysicsdirector()
{
    gravity = glm::vec3{0,-9.81,0};
    physics_delta = 0;
    prev_time = 0;
    delta_time = 0.0;
    set_initial_search(glm::vec3(1, 10, 1));
}
void Myphysicsdirector::start_sim()
{
    prev_time = glfwGetTime();
}
void Myphysicsdirector::_new_elements_added()
{
    collision_recording.resize(axes.size() * axes.size());
}
void Myphysicsdirector::update_physics()
{
    double nowTime = glfwGetTime();
    if (prev_time == 0)
    {
        prev_time = nowTime;
        return;
    }
    delta_time += nowTime-prev_time;
    prev_time = nowTime;
    while (delta_time >= TIMEDELTA)
    {
        _physics_process(static_cast<float>(TIMEDELTA));
        //delta_time = 0.0;
       delta_time -= TIMEDELTA;
    }
    //_physics_process(delta_time);
    //delta_time = 0;

}
;

void Myphysicsdirector::_process(float delta)
{
    if(axes.empty()) return;
    if(physicson)
        return;
    collision_recording_size=0;
    for(unsigned i=0;i< axes.size(); ++i)
    {
        axes[i].get().is_colliding = false;
    }
    for(unsigned i=0;i< axes.size(); ++i)
    {
        for(unsigned j=i+1;j< axes.size();++j)
        {
                temporary_epa_buffer=epa_record();
                if(gjk(axes[i],axes[j],true))
                {
                    axes[i].get().is_colliding = true;
                    axes[j].get().is_colliding=true;
                    collision_recording[collision_recording_size++] = epa(axes[i],axes[j]);
                    temporary_epa_buffer=epa_record();
                }
                else
                {
                    
                }
        }
    }
    for(unsigned ind=0; ind < collision_recording_size;++ind)
    {
        auto & currentcolrecord =collision_recording[ind];
        auto tmpa = collision_recording[ind].a;
        auto tmpb = collision_recording[ind].b;
        if(currentcolrecord.polytope.face_list.empty())
        {
            //unable to get collision point in this case
            //point direction //move the the direction into the center of mass of a and b
            //Journal article there is documentation.
            //archives
            //
                continue;
        }
        auto tmpnorm = glm::normalize(collision_recording[ind].norm);
        if (tmpnorm.x*tmpnorm.x <=0.001)
            tmpnorm.x=0;
        if (tmpnorm.y*tmpnorm.y <=0.001)
            tmpnorm.y=0;
        if (tmpnorm.z*tmpnorm.z <=0.001)
            tmpnorm.z=0;
        collision_recording[ind].polytope.face_list[0]._1;
        auto tmpdistance = collision_recording[ind].depth;//collision_recording[ind].polytope.face_list[0]._1.sum.dot(tmpnorm);
        auto tmmmmp =collision_recording[ind].bcenter.pointOnSecond;
        auto tmmmmp1 =collision_recording[ind].bcenter.pointOnFirst;
        auto ap = tmpa->get_position();
        auto bp = tmpb->get_position();
        auto rotVaLine = tmmmmp1 - ap;
        auto rotVbLine = tmmmmp - bp;
    }
}
void Myphysicsdirector::_physics_process(float delta)
{
    //std::cout << "sphere collider axes size: " << axes[0].get().axes.size() << std::endl;
    if(!physicson)
    {
        return;
    }
    
    //if(not_step_mode || stepped)
    //{   
    //    stepped=false;
    //}
    //else
    //{
    //    stepped = false;
    //    return;
    //}
 /*now time to figure out how to do this for children*/
    //set_position(get_position()+gravity*delta);
    //modified euler step
    //consider acceleration without gravity first?
    //apply gravity
    //update this shit.
    // whatever we make children be predestined for now.

    collision_recording_size=0;
    glm::vec3 grav_modifier= gravity*delta;
    float deltahalf= delta/2.0f;
    
    for(auto each:axes)
    {
        collider& tmpp = each.get(); // I removed the &
        //[TODO] UPDATE THIS TERM
        each.get().is_colliding = false;
        //tmpp.transformRef = static_cast<TransformComponent>(tmpp);
        //std::cout << "tmpp is_affected_by_gravity:" << tmpp.is_affected_by_gravity;
        //std::cout << "tmpp transform: " << tmpp.GetPosition().x << ", " <<
            //tmpp.GetPosition().y << ", " << tmpp.GetPosition().z << std::endl;
        if (!tmpp.is_affected_by_gravity) continue;
        glm::vec3 futureV= tmpp.get_velocity()+tmpp.get_acceleration()*delta+grav_modifier;
        glm::vec3 futureP= tmpp.get_global_position()+deltahalf*(futureV+tmpp.get_velocity());
        //heuns method
        
        auto temporary = tmpp.get_principleAxisRotationVelocity();
        
        //[TODO] UPDATE THIS TERM PRESERVATION OF ANGULAR MOMENTUM IS NOT ACCOUNTED FOR HERE
        //tmpp.set_principleAxisRotationVelocity(temporary);
        //auto invinertiab=tmpp.get_my_basis();
        tmpp.set_global_position(futureP);
        tmpp.set_velocity(futureV);
        if (tmpp.get_rotatable())
        {
            //glm::matrix_cross_product::matrixCross4
           // apply_AngularVelocity
            tmpp.apply_AngularVelocity(delta);
        }
        
        //tmpp.rotate_x(temporary.x*delta);
        //tmpp.rotate_y(temporary.y*delta);
        //tmpp.rotate_z(temporary.z*delta);
        
    }
    //return;
    /*for(unsigned i=0;i< axes.size(); ++i)
    {
        axes[i]->is_colliding=false;
    }*/
    //THIS is the collision detection loop to upgrade to make it a little more uh sophisticated maybe
    //

    //int count = 0;
   // double nowTime = glfwGetTime();
    for(unsigned i=0;i< axes.size(); ++i)
    {
        collider& a = axes[i].get();
        for(unsigned j=i+1;j< axes.size();++j)
        {
            collider& b = axes[j].get();
            temporary_epa_buffer=epa_record();
            if(a.is_affected_by_gravity || b.is_affected_by_gravity)
            if(gjk(a,b,true))
            {
                //++count;
                    
                    a.is_colliding=true;
                    b.is_colliding=true;
                    collision_recording[collision_recording_size++] = epa(axes[i],axes[j]);
                    //TODO LOOK AT HOW TO OPTIMIZE THIS FOR NON DYNAMIC COLLISIONS)
                    //temporary_epa_buffer=epa_record();
            }
        }
    }
    
   // std::cout << nowTime- glfwGetTime() << std::endl;
    //return;
    //physics resolution`
    for(unsigned ind=0; ind < collision_recording_size;++ind)
    {
        //resolution
        auto & currentcolrecord = collision_recording[ind];
        auto tmpa = currentcolrecord.a;
        auto tmpb = currentcolrecord.b;
        if(currentcolrecord.polytope.face_list.empty())
        {
            //unable to get collision point in this case
            //point direction //move the the direction into the center of mass of a and b
            //Journal article there is documentation.
            //archives
            //
                continue;
        }
        auto tmpnorm = glm::normalize(currentcolrecord.norm);
        //norm always points towards b
        
        
        auto tmpdistance = glm::dot(currentcolrecord.polytope.face_list[0]._1.sum,tmpnorm);
        auto tmmmmp =currentcolrecord.bcenter.pointOnSecond;
        auto tmmmmp1 =currentcolrecord.bcenter.pointOnFirst;
        auto ap = tmpa->get_global_position();//todo implement get global position
        auto bp = tmpb->get_global_position();//todo implement get global position
        //::cout << glm::dot(bp-ap, tmpnorm) << std::endl;
      /*  tmmmmp=tmmmmp+tmmmmp1;
        tmmmmp= 0.5f*tmmmmp;
        tmmmmp1 =tmmmmp;*/
        
        auto rotVaLine = tmmmmp1 - ap;
        
        auto rotVbLine = tmmmmp - bp;
        auto omegaxra= glm::cross(tmpa->get_principleAxisRotationVelocity(),rotVaLine);
        auto omegaxrb= glm::cross(tmpb->get_principleAxisRotationVelocity(),rotVbLine);
        auto va = tmpa->get_velocity();
        auto vb = tmpb->get_velocity();
        
        auto collisionpointAVel=omegaxra + va;
        
        auto collisionpointBVel=omegaxrb + vb;
        auto vp1= collisionpointAVel - collisionpointBVel;
        float divisor = 1.0000f;
        if(tmpa->is_affected_by_gravity)
            if(tmpb->is_affected_by_gravity)
                divisor =0.5f;

        auto vr_t_a = glm::dot(collisionpointAVel, tmpnorm);
        
        auto t_hat_a = collisionpointAVel - vr_t_a * tmpnorm;
        auto l_t_hat_a = t_hat_a.length();
        if (l_t_hat_a != 0.0)
            t_hat_a = 1.0f/ l_t_hat_a*t_hat_a;
        auto vr_t_b = glm::dot(collisionpointBVel, -tmpnorm);
        auto t_hat_b = collisionpointBVel + vr_t_b* tmpnorm;
        auto l_t_hat_b = t_hat_b.length();
        if (l_t_hat_b != 0.0)
            t_hat_b = 1.0f / l_t_hat_b * t_hat_b;

        if(glm::dot(tmpnorm,vp1)<0.0f)
        {
            //if this is negative  this means im moving away already so theres no need to resolve the colision.
            //[TODO] MODIFY HOW THIS WORKS
            //this is penetration resolution.
            
            if(tmpa->is_affected_by_gravity)
            tmpa->set_global_position(tmpa->get_global_position()-(divisor*tmpdistance)*tmpnorm);
            if(tmpb->is_affected_by_gravity)
            tmpb->set_global_position(tmpb->get_global_position()+(divisor*tmpdistance)*tmpnorm);

            continue;
            
        }



        // LOOK AT restitution formula
        float e = tmpa->get_restitution()*tmpb->get_restitution();
       
        auto jtop = -(1.0f+e)*(glm::dot(vp1,tmpnorm));
        auto thisistmpa= glm::cross(rotVaLine,tmpnorm);
        auto thisistmpb= glm::cross(rotVbLine,tmpnorm);
        
        auto invinertiaa= tmpa->get_my_basis();
        auto invinertiab= tmpb->get_my_basis();
        auto inertiaa =1.0f;
        auto inertiab =1.0f;
        auto jbot =tmpa->get_inv_mass()+ tmpb->get_inv_mass();
         if(tmpa->is_affected_by_gravity)
         {
            jbot+=glm::dot(thisistmpa,((invinertiaa*thisistmpa)));
         }
         if(tmpb->is_affected_by_gravity)
         {
            
             jbot+= glm::dot(thisistmpb,((invinertiab*thisistmpb)));
         }
         auto j = jtop/jbot;
         //relative velocity
         auto delv= vb-va;
        
         auto tmeep = glm::dot(tmpnorm,delv); //relative velocity from a so it needs to be positive
         if(isnan(j))
          {
              j=0.0f;
              if(tmpa->is_affected_by_gravity)
                 {
                     va = va + divisor*tmeep*tmpnorm;
                     tmpa->set_velocity(va);
                     tmpa->set_position(tmpa->get_position()-(divisor*tmpdistance)*tmpnorm);
                  }
             if(tmpb->is_affected_by_gravity)
                  {
                  vb= vb - divisor*tmeep*tmpnorm;
                  tmpa->set_velocity(vb);
                  tmpb->set_position(tmpb->get_position()+(divisor*tmpdistance)*tmpnorm);
                 }
             continue;

         }
        //
        /*OLD VELOCITY REFORM*/
        /*fake friction*/
        /* */
        float frictionmultipliers = tmpa->static_friction * tmpb->static_friction;
        float frictionmultiplierk = tmpa->kinetic_friction * tmpb->kinetic_friction;
        float jrs = frictionmultipliers*j;
        float jrk = frictionmultiplierk*j;
        
        
        va = va + j*tmpa->get_inv_mass()*tmpnorm;
        vb = vb - j*tmpb->get_inv_mass()*tmpnorm;
        //auto jn = tmeep*j*tmpnorm;

        
//td::cout << "pair start" << std::endl;
        if(tmpa->is_affected_by_gravity)
        {
            float vrta = glm::dot(collisionpointAVel, t_hat_a);
            //d::cout <<"a" << vrta << "MASH" << tmpa->get_mass() << std::endl;
            auto jfa = (tmpa->get_mass() * vrta < jrs) ? -vrta * t_hat_a : -jrk*tmpa->get_inv_mass() * t_hat_a;
            va -= jfa;
             tmpa->set_velocity(va);
             // modify 
             tmpa->set_principleAxisRotationVelocity(tmpa->get_principleAxisRotationVelocity()+((j)*(invinertiaa*thisistmpa)));
             //tmpa->update_principleAxisRotationVelocity(((j)*invinertiaa.xform(thisistmpa)));

             tmpa->set_global_position(tmpa->get_global_position()-(divisor*tmpdistance)*tmpnorm);
        }
        if(tmpb->is_affected_by_gravity)
        {
                float vrtb = glm::dot(collisionpointBVel, t_hat_b);
                //d::cout << "b" << vrtb <<"MASH"<< tmpb->get_mass() << std::endl;
                auto jfb = (tmpb->get_mass() * vrtb < jrs) ? -vrtb * t_hat_b : -jrk*tmpb->get_inv_mass() * t_hat_b;
                vb -= jfb;
                tmpb->set_velocity(vb);
                tmpb->set_principleAxisRotationVelocity(tmpb->get_principleAxisRotationVelocity()-((j)*(invinertiab*thisistmpb)));
                //tmpb->update_principleAxisRotationVelocity(((-j)*invinertiaa.xform(thisistmpb)));
                tmpb->set_global_position(tmpb->get_global_position()+(divisor*tmpdistance)*tmpnorm);
        }
    }
}
collider* Myphysicsdirector::_raycast(const glm::vec3 dir, const glm::vec3 pos, float max_ray)
{
    // dont need to do epa
    //oh i know what to do.
    // so you can do this size way.
    //collision points.
    // so we need to define a ray.
    
    // so now i want to create two gjk algorithms.
    prim_half_line tes(dir,max_ray);
    TransformComponent rayTransform = TransformComponent(pos,glm::vec3(0.0f),glm::vec3(1.0f));
    collider ray(rayTransform);
    ray.axes.emplace_back(Ref(tes));
    epa_record temporary = epa_record();
    temporary.a = &ray;
    temporary.b = nullptr;
    for (unsigned i = 0; i < axes.size(); ++i)
    {
        auto previousclosest = temporary.b;
        temporary.b = &(axes[i].get());
        temporary.polytope = polytope_prim();
        //there really isnt a need for collision point
        if (on_demand_gjk(temporary))
        {
            max_ray = std::min(glm::distance(pos, on_demand_epa(temporary).bcenter.pointOnFirst), max_ray);
        }
        else
        {
            temporary.b = previousclosest;
        }
    }
    return temporary.b;
}
void Myphysicsdirector::set_gravity(const glm::vec3 p_gravity)
{
    gravity = p_gravity;
}
glm::vec3 Myphysicsdirector::get_gravity() const
{
    return gravity;
}
void Myphysicsdirector::set_initial_search(const glm::vec3 p_initial_search)
{
    initial_search = glm::normalize(p_initial_search);
}
glm::vec3 Myphysicsdirector::get_initial_search() const {
	return initial_search;
}
void Myphysicsdirector::set_physicson(const bool p_physicson) {
	physicson = p_physicson;

}
bool Myphysicsdirector::get_physicson() const
{
	return physicson;
}

void Myphysicsdirector::set_dbg(const bool p_dbg) {
	dbg = p_dbg;
}
bool Myphysicsdirector::get_dbg() const
{
	return dbg;
}

void Myphysicsdirector::set_epsi(const float p_epsi)
{
    
    if((p_epsi > 0.0)|| p_epsi<=0)
    {
        epsi=0.000001f;
        return;
    }
    epsi=p_epsi;
}

float Myphysicsdirector::get_epsi() const
{
	return epsi;
}




bool Myphysicsdirector::gjk(collider& a, collider& b, bool debugmess)
{
    simplex simp;
    //glm::vec3 direct = initial_search;
    pointcombo tmp = support(a,b, initial_search);
    simp.push_front(tmp,debugmess);
    for(glm::vec3 D = -tmp.sum;true;)
    {
       tmp = support(a,b,D);
       if (glm::dot(D, tmp.sum) <= 0)
       {
         //im going to go off on a limb and say this is important information that needs to be stored
         //CURRENT FRONT IS CLOSEST TO ORIGIN.
         return false;
       }
       simp.push_front(tmp,debugmess);
       if(gjk_next(simp,D))
       {
          temporary_epa_buffer.a=&a;
          temporary_epa_buffer.b=&b;
          temporary_epa_buffer.norm=D;
          temporary_epa_buffer.polytope=polytope_prim(simp);
          return true;
       }
    }
    return false;
}

col_rec Myphysicsdirector::epa(collider& a, collider& b)
{
    // create array.
    //search direction 
    //convert into edge format and indice the points
    glm::vec3 minD;
    col_rec col_recordio;
    col_recordio.a=&a;
    col_recordio.b=&b;
    unsigned countd=0;
    double minDis = std::numeric_limits<double>::max();
    while (minDis == std::numeric_limits<double>::max())
        {
            ++countd;
            auto &tmp =temporary_epa_buffer.polytope.face_list[0];
            //
            minD = tmp.norm;
            //minD=glm::normalize(tmp.norm);
            minDis = tmp.distance_origin;
            auto supoint = support(a,b,minD);
            double suDis = glm::dot(supoint.sum, minD);
            double diff = minDis - suDis;
            /*As long as we are still searching for new faces that are still different from the current face*/
            if(signbit(diff))
            {
                diff*=-1.0f;
            }
            if(diff > epsi)
            {
                minDis = std::numeric_limits<double>::max();
                if(temporary_epa_buffer.polytope.push(supoint))
                {
                    //Should never reach this condition.
                    break;
                }
                if(countd> ITERATION_MAX_EPA)
                {
                    //This was the average I found online that would be good enough apparently
                    break;
                }
            }
        }
     
    //temporary_epa_buffer.polytope.sort();
    col_recordio.norm=temporary_epa_buffer.polytope.face_list.front().norm;
    col_recordio.depth = temporary_epa_buffer.polytope.face_list.front().distance_origin;
    col_recordio.bcenter=barycenter(temporary_epa_buffer.polytope.face_list.front());
    col_recordio.polytope = temporary_epa_buffer.polytope;
    return col_recordio;
}

bool Myphysicsdirector::gjk_next(simplex &pts, glm::vec3 &dir)
{
            //self explanatory
			switch(pts.m_size)
			{
				case 2:
					return gjk_line(pts,dir);
				case 3:
					return gjk_tri(pts,dir);
				case 4:
					return gjk_tetra(pts,dir);
				default:
				    return false;
			}
}

bool Myphysicsdirector::gjk_line(simplex &pts, glm::vec3 &dir)
{
    pointcombo a = pts.points[0];
    const glm::vec3 ab =pts.points[1].sum-a.sum;
    const glm::vec3 ao = -a.sum;
    if(dircheck(ab,ao))
    {
        //this provides the vector shortest distance of line to origin
        //essentially towards ao hence towards origin
        dir = glm::cross(glm::cross(ab,ao),ab);//Towards ao ish.
        //this searches in the direction of the origin since we always start with a and b being opposing sides of the origin
    }
    else
    {
        //this means collinear
        pts = {a};
        dir = ao;
    }
	return false;
}

bool Myphysicsdirector::gjk_tri(simplex &pts, glm::vec3 &dir) {
    
	pointcombo a = pts.points[0];
	pointcombo b = pts.points[1];
    pointcombo c = pts.points[2];
    glm::vec3 ab =b.sum-a.sum;
    glm::vec3 ac =c.sum-a.sum;
    glm::vec3 ao = -a.sum;
    //plane direction
    glm::vec3 abxac= glm::cross(ab,ac);
    if(dircheck(glm::cross(abxac,ac),ao))
    {
        //this provides the vector shortest distance of line to origin
        if(dircheck(ac,ao))
        {
            //this means origin is away from ab compared to oa therefore we get rid of b
            //shortest distance to the line again so search that direction to enclose origin
            pts={a,c};
            dir = glm::cross(glm::cross(ac,ao),ac);

            // a x b x a means perp from a towards b
            // b x a x a means perp from a away from b
        }
        else
        {
            //this means B is closer to the origin since ao is pointing away from ac
            pts ={a,b};
            return gjk_line(pts,dir);
        }
    }
    else if(dircheck(glm::cross(ab,abxac),ao))
    {
        pts={a,b};
        return gjk_line(pts,dir);
    }
    else if(dircheck(abxac,ao))
    {
            //enclosing origin
            dir = abxac;
    }
    else
    {
            //same condition as previous just twizzle because its on the wrong side of our triangle that wants to enclose origin
            pts={a,c,b};
            dir = -abxac;
    }
        
     
    return false;
}

bool Myphysicsdirector::gjk_tetra(simplex &pts, glm::vec3 &dir) {
    pointcombo a = pts.points[0];
    pointcombo b = pts.points[1];
    pointcombo c = pts.points[2];
    pointcombo d = pts.points[3];
    glm::vec3 ab =b.sum-a.sum;
    glm::vec3 ac =c.sum-a.sum;
    glm::vec3 ad =d.sum-a.sum;
    glm::vec3 ao = -a.sum;
    //glm::vec3 abxac= glm::cross(ab,ac);
    //glm::vec3 acxad= glm::cross(ac,ad);
   // glm::vec3 adxab= glm::cross(ad,ab);
    if(dircheck(glm::cross(ab, ac),ao))
    {
        pts= {a,b,c};
        //outside the abc face
        return gjk_tri(pts ,dir);
    }
    if(dircheck(glm::cross(ac, ad),ao))
    {
        //outside the acd face
        pts={a,c,d};
        return gjk_tri(pts,dir);
    }
    if(dircheck(glm::cross(ad, ab),ao))
    {
        //outside the adb face
        pts={a,d,b};
        return gjk_tri(pts,dir);
    }
    // need to add some epsilon
    return true;
}

pointcombo Myphysicsdirector::support(collider& a, collider& b, const glm::vec3 & direction)
{
    return pointcombo(a.find_furthest(direction),b.find_furthest(-direction));
}

//may want to update information being passed around but this will do for now
bool Myphysicsdirector::on_demand_gjk(epa_record& col_pair)
{
    auto a = col_pair.a;
    auto b = col_pair.b;

    if ((!a) || (!b)) return false;
    simplex simp;
    //glm::vec3 direct = ; //TODO MAKE THIS EASILY MODFIABLE
    pointcombo tmp = support(*a, *b, initial_search);
    simp.push_front(tmp);
    col_pair.norm = -tmp.sum;
    glm::vec3& D = col_pair.norm;
    for (; true;)
    {
        pointcombo tmpp = support(*a, *b, D);
        if (glm::dot(D, tmpp.sum) <= 0)
        {
            //im going to go off on a limb and say this is important information that needs to be stored
            //CURRENT FRONT IS CLOSEST TO ORIGIN.
            //this implies in its current form the simplex is correct.
            col_pair.polytope = polytope_prim(simp);
            //since we always create the simplex of the closest feature.
            return false;
        }
        simp.push_front(tmpp);
        if (gjk_next(simp, D))
        {
            //simp.push_front(tmpp);
            col_pair.polytope = polytope_prim(simp);
            return true;
        }
    }
    return false;
}



col_rec Myphysicsdirector::on_demand_epa(epa_record& col_pair)
{
    // create array.
    //search direction 
    //convert into edge format and indice the points
    glm::vec3 minD;
    col_rec col_recordio;
    col_recordio.a = col_pair.a;
    col_recordio.b = col_pair.b;
    unsigned countd = 0;
    double minDis = std::numeric_limits<double>::max();
    while (minDis == std::numeric_limits<double>::max())
    {
        ++countd;
        auto& tmp = col_pair.polytope.face_list[0];
        //
        minD = glm::normalize(tmp.norm);
        minDis = tmp.distance_origin;
        auto supoint = support(*(col_pair.a), *(col_pair.b), minD);
        double suDis = glm::dot(supoint.sum, minD);
        double diff = minDis - suDis;
        /*As long as we are still searching for new faces that are still different from the current face*/
        if (signbit(diff))
        {
            diff *= -1.0f;
        }
        if (diff > epsi)
        {

            minDis = std::numeric_limits<double>::max();
            if (col_pair.polytope.push(supoint))
            {
                //Should never reach this condition.
                break;
            }
            if (countd > ITERATION_MAX_EPA)
            {
                //This was the average I found online that would be good enough apparently
                break;
            }
        }
    }

    col_pair.polytope.sort();
    col_recordio.norm = col_pair.polytope.face_list.front().norm;
    col_recordio.depth = col_pair.polytope.face_list.front().distance_origin;
    col_recordio.bcenter = barycenter(col_pair.polytope.face_list.front());
    col_recordio.polytope = col_pair.polytope;
    return col_recordio;
}

void Myphysicsdirector::CreatePhysicsDirectorLuaBind(std::shared_ptr<sol::state> lua) {
    // bind 
    
    // bind col_rec to lua
    lua->new_usertype<col_rec>("col_rec",
        "a", &col_rec::a,
        "b", &col_rec::b
        );
    
    // bind std::vector<Ref<collider>> to lua
    lua->new_usertype<std::vector<Ref<collider>>>("collider_axes",
        "emplace_back", [](std::vector<Ref<collider>>& v, collider& value) { 
            v.emplace_back(Ref(value)); 
        },
        "push_back", [](std::vector<Ref<collider>>& v, collider& value) {
            v.push_back(Ref(value));
            std::cout << "my physics director std::vector<Ref<collider>> push_back function is called" << std::endl;
        }
    );

    // bind std::vector<col_rec> to lua

    lua->new_usertype<Myphysicsdirector>("PhysicsDirector",
        "axes", &Myphysicsdirector::axes,
        "collision_recording", &Myphysicsdirector::collision_recording,
        "num_collision", &Myphysicsdirector::collision_recording_size);
}

