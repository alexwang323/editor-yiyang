#ifndef MYPHYSICSDIRECTOR_H
#define MYPHYSICSDIRECTOR_H
#include "collider.h"
#include <vector>
#include <array>
#include<algorithm>
#include<set>
#include<cmath>
#include <sol/sol.hpp>
#include "RefWrapper.h"
struct pointcombo
{
    glm::vec3 a;
    glm::vec3 b;
    glm::vec3 sum;
    pointcombo(glm::vec3 _a=glm::vec3(), glm::vec3 _b=glm::vec3()):a(_a),b(_b),sum(_a-_b)
    {

    }
};

struct edge_prim
{
    std::pair<unsigned,unsigned> edgey;
    edge_prim(unsigned _a=0, unsigned _b=0)
    {
        if(_a<_b)
        {
            edgey={_a,_b};
        }
        else
        {
            edgey={_b,_a};
        }
    };
};



struct faces
{
    pointcombo _1;
    pointcombo _2;
    pointcombo _3;
    unsigned ind_1;
    unsigned ind_2;
    unsigned ind_3;
    glm::vec3 norm;
    float distance_origin;
    glm::vec3 closest_point;
    //adapted from https://stackoverflow.com/questions/2924795/fastest-way-to-compute-point-to-triangle-distance-in-3d
    glm::vec3 cPT(pointcombo a=pointcombo(),pointcombo b=pointcombo(),pointcombo c=pointcombo())
    {
        const glm::vec3 ab = b.sum - a.sum;
        const glm::vec3 ac = c.sum - a.sum;
        const glm::vec3 ap = -a.sum;
        const float d1 = glm::dot(ab,ap);
        const float d2 = glm::dot(ac,ap);
        if (d1 <= 0.f && d2 <= 0.f) return a.sum; //#1
        const glm::vec3 bp =  - b.sum;
        const float d3 = glm::dot(ab,bp);
        const float d4 = glm::dot(ac,bp);
        if (d3 >= 0.f && d4 <= d3) return b.sum; //#2
        const glm::vec3 cp =  - c.sum;
        const float d5 = glm::dot(ab, cp);
        const float d6 = glm::dot(ac, cp);
        if (d6 >= 0.f && d5 <= d6) return c.sum; //#3
        const float vc = d1 * d4 - d3 * d2;
        if (vc <= 0.f && d1 >= 0.f && d3 <= 0.f)
        {
            const float v = d1 / (d1 - d3);
            return a.sum + v * ab; //#4
        }
        const float vb = d5 * d2 - d1 * d6;
        if (vb <= 0.f && d2 >= 0.f && d6 <= 0.f)
        {
            const float v = d2 / (d2 - d6);
            return a.sum + v * ac; //#5
        }
        const float va = d3 * d6 - d5 * d4;
        if (va <= 0.f && (d4 - d3) >= 0.f && (d5 - d6) >= 0.f)
        {
            const float v = (d4 - d3) / ((d4 - d3) + (d5 - d6));
            return b.sum + v * (c.sum - b.sum); //#6
        }
        const float denom = 1.f / (va + vb + vc);
        const float v = vb * denom;
        const float w = vc * denom;
        return a.sum + v * ab + w * ac; //#0
    }
    faces(pointcombo a=pointcombo(),pointcombo b=pointcombo(),pointcombo c=pointcombo(),unsigned _a=0,unsigned _b=0, unsigned _c = 0):_1(a),_2(b),_3(c),ind_1(_a),ind_2(_b),ind_3(_c),norm(glm::normalize(glm::cross(b.sum-a.sum,c.sum-a.sum)))
    {
        distance_origin = glm::dot(_1.sum,norm);
        if(distance_origin < 0)
        {
            std::swap(_2,_3);
            std::swap(ind_2,ind_3);
            norm=glm::normalize(glm::cross(c.sum-a.sum,b.sum-a.sum));
            
            distance_origin = glm::dot(_1.sum,norm);
        }
        closest_point = cPT(a,b,c);
        distance_origin= sqrt(glm::dot(closest_point,closest_point));
    }
    faces(pointcombo a,pointcombo b,pointcombo c,pointcombo d,unsigned _a=0,unsigned _b=0, unsigned _c = 0):_1(a),_2(b),_3(c),ind_1(_a),ind_2(_b),ind_3(_c),norm(glm::normalize(glm::cross(b.sum-a.sum,c.sum-a.sum)))
    {
        //this is designed to point away from a point.
        distance_origin = glm::dot(norm,(d.sum-a.sum));
        if(distance_origin > 0)
        {
            std::swap(_2,_3);
            std::swap(ind_2,ind_3);
            norm=glm::normalize(glm::cross(c.sum-a.sum,b.sum-a.sum));
            
            
        }
        closest_point = cPT(a,b,c);
        distance_origin= sqrt(glm::dot(closest_point,closest_point));
    }
    glm::vec3 get_norm() const
    {
        return norm;
    }
};


struct simplex
{
    std::array<pointcombo,4> points;
    unsigned m_size;
    simplex(): points(),m_size(0)
    {

    }
    simplex & operator=(std::initializer_list<pointcombo> initlist)
    {
        
        m_size = 0;
        for(pointcombo tmp: initlist)
            points[m_size++]=tmp;
        return *this;
    }
    inline void push_front(const pointcombo& pt, bool test=false)
    {
        points[3]=points[2];
        points[2]=points[1];
        points[1]=points[0];
        points[0]=pt;
        m_size = std::min<unsigned>(m_size+1,4);
    }
    
};

struct polytope_prim
{
    std::vector<pointcombo> points;
    std::vector<faces> face_list;
    void sort()
    {
        std::sort(face_list.begin(), face_list.end(), [](faces a, faces b)
                                  {
                                      return (a.distance_origin) < (b.distance_origin);
                                  });
    }
    bool push(pointcombo a)
    {
        std::set<std::pair<unsigned,unsigned>>edgeys;
        std::set<std::pair<unsigned,unsigned>>edgeys_remowed;
        //std::set<std::pair<unsigned, unsigned>>edgeys_visit;
        unsigned index = static_cast<unsigned>(points.size());
        points.push_back(a);
        //push points in
        if(face_list.size())
        {
            std::vector<faces> face_list_tmp;
            //face_list_tmp.reserve(face_list.size());
            //face_list_tmp.reserve
            for(auto &f: face_list)
            {
                if(glm::dot(f.norm,a.sum-f._1.sum)<0)
                {
                    face_list_tmp.push_back(f);
                    //these face the opposite way so are unaffected
                }
                else
                {
                    //checking edges if they have been seen before
                    auto edger = edge_prim(f.ind_1,f.ind_2).edgey;
                   
                    if(edgeys.count(edger))
                    {
                        edgeys.erase(edger);
                        edgeys_remowed.insert(edger);
                    }
                    else if(!edgeys_remowed.count(edger))
                    {
                        edgeys.insert(edger);
                    }


                    edger = edge_prim(f.ind_2,f.ind_3).edgey;
                    if(edgeys.count(edger))
                    {
                        edgeys.erase(edger);
                        edgeys_remowed.insert(edger);
                    }
                    else if(!edgeys_remowed.count(edger))
                    {
                        edgeys.insert(edger);
                    }
                    edger = edge_prim(f.ind_3,f.ind_1).edgey;
                    if(edgeys.count(edger))
                    {
                        edgeys.erase(edger);
                        edgeys_remowed.insert(edger);
                    }
                    else if(!edgeys_remowed.count(edger))
                    {
                        edgeys.insert(edger);
                    }
                }
            }
            for(auto& adge: edgeys)
                {
                        auto a1 =adge.first;
                        auto a2 =adge.second;
                        face_list_tmp.push_back(faces(points[a1],points[a2],a,a1,a2,index));
                }
            if(face_list_tmp.empty())
            {
                sort();
                return true; //need to return point dumbass
            } 
            std::swap(face_list,face_list_tmp);
            sort();
            return false;
        }
        else
        {
            if(points.size() == 4)
            {
                //create faces
                //face_list.clear();
                face_list.push_back(faces(points[0],points[1],points[2],points[3],0,1,2));
                face_list.push_back(faces(points[1],points[2],points[3],points[0],1,2,3));
                face_list.push_back(faces(points[0],points[3],points[2],points[1],0,3,2));
                face_list.push_back(faces(points[0],points[1],points[3],points[2],0,1,3));
                sort();
                
                
            }
            return false;
        }
    }
    void push(faces fc)
    {
        face_list.push_back(fc);
    }
    polytope_prim(){};
    polytope_prim(simplex a)
    {
        for (auto& pnt : a.points) { push(pnt); };
    }
    
};


//adapted from https://stackoverflow.com/questions/2924795/fastest-way-to-compute-point-to-triangle-distance-in-3d
struct barycenter
{
    pointcombo _A;
    pointcombo _B;
    pointcombo _C;
    glm::vec3 pointOnFirst;
    glm::vec3 pointOnSecond;
    
    barycenter(pointcombo a=pointcombo(),pointcombo b=pointcombo(),pointcombo c=pointcombo()):_A(a),_B(b),_C(c)
    {
        const glm::vec3 ab = b.sum - a.sum;
        const glm::vec3 ac = c.sum - a.sum;
        const glm::vec3 ap = -a.sum;
        const float d1 = glm::dot(ab,ap);
        const float d2 = glm::dot(ac,ap);
        if (d1 <= 0.f && d2 <= 0.f) 
        {
            pointOnFirst=a.a;
            pointOnSecond=a.b;
            return;
        } //#1
        const glm::vec3 bp =  - b.sum;
        const float d3 = glm::dot(ab,bp);
        const float d4 = glm::dot(ac,bp);
        if (d3 >= 0.f && d4 <= d3)
        {
            pointOnFirst=b.a;
            pointOnSecond=b.b;
            return;
        }      //#2
        const glm::vec3 cp =  - c.sum;
        const float d5 = glm::dot(ab, cp);
        const float d6 = glm::dot(ac,cp);
        if (d6 >= 0.f && d5 <= d6)
        {
            pointOnFirst=c.a;
            pointOnSecond=c.b;
            return;
        } //#3
        const float vc = d1 * d4 - d3 * d2;
        if (vc <= 0.f && d1 >= 0.f && d3 <= 0.f)
        {
            const float v = d1 / (d1 - d3);
            pointOnFirst=a.a+v*(b.a-a.a);
            pointOnSecond=a.b+v*(b.b-a.b);
            return; //#4
        }
        const float vb = d5 * d2 - d1 * d6;
        if (vb <= 0.f && d2 >= 0.f && d6 <= 0.f)
        {
            const float v = d2 / (d2 - d6);
            pointOnFirst=a.a+v*(c.a-a.a);
            //std::roundf
            pointOnSecond=a.b+v*(c.b-a.b);
            return; //#5
        }
        
        const float va = d3 * d6 - d5 * d4;
        if (va <= 0.f && (d4 - d3) >= 0.f && (d5 - d6) >= 0.f)
        {
            const float v = (d4 - d3) / ((d4 - d3) + (d5 - d6));
            pointOnFirst =b.a + v*(c.a-b.a);
            pointOnSecond=b.b + v*(c.b-b.b);
            return; //#6
        }
        
        const float denom = 1.f / (va + vb + vc);
        const float v = vb * denom;
        const float w = vc * denom;
        pointOnFirst =a.a+ v*(b.a-a.a) + w*(c.a-a.a);
        pointOnSecond=a.b+ v*(b.b-a.b) + w*(c.b-a.b);
        return; //#0
    }
    barycenter(faces hi):_A(hi._1),_B(hi._2),_C(hi._3)
    {
        auto a=_A;
        auto b = _B;
        auto c= _C;
  const glm::vec3 ab = b.sum - a.sum;
  const glm::vec3 ac = c.sum - a.sum;
  const glm::vec3 ap = -a.sum;

  const float d1 = glm::dot(ab,ap);
  const float d2 = glm::dot(ac,ap);
  if (d1 <= 0.f && d2 <= 0.f) 
  {
      pointOnFirst=a.a;
      pointOnSecond=a.b;
      return;
  }; //#1

  const glm::vec3 bp =  - b.sum;
  const float d3 = glm::dot(ab,bp);
  const float d4 = glm::dot(ac,bp);
  if (d3 >= 0.f && d4 <= d3)
  {
      pointOnFirst=b.a;
      pointOnSecond=b.b;
      return;
  };      //#2

  const glm::vec3 cp =  - c.sum;
  const float d5 = glm::dot(ab,cp);
  const float d6 = glm::dot(ac,cp);
  if (d6 >= 0.f && d5 <= d6)
  {
      pointOnFirst=c.a;
      pointOnSecond=c.b;
      return;
  }; //#3

  const float vc = d1 * d4 - d3 * d2;
  if (vc <= 0.f && d1 >= 0.f && d3 <= 0.f)
  {
      const float v = d1 / (d1 - d3);
      pointOnFirst=a.a+v*(b.a-a.a);
      pointOnSecond=a.b+v*(b.b-a.b);
      return; //#4
  }
    
  const float vb = d5 * d2 - d1 * d6;
  if (vb <= 0.f && d2 >= 0.f && d6 <= 0.f)
  {
      const float v = d2 / (d2 - d6);
      pointOnFirst=a.a+v*c.a-v*a.a;
        //std::roundf
     pointOnSecond=a.b+v*c.b-v*a.b;
      return; //#5
  }
    
  const float va = d3 * d6 - d5 * d4;
  if (va <= 0.f && (d4 - d3) >= 0.f && (d5 - d6) >= 0.f)
  {
      const float v = (d4 - d3) / ((d4 - d3) + (d5 - d6));
      pointOnFirst =b.a + v*(c.a-b.a);
      pointOnSecond=b.b + v*(c.b-b.b);
      return; //#6
  }

  const float denom = 1.f / (va + vb + vc);
  const float v = vb * denom;
  const float w = vc * denom;
  pointOnFirst =a.a+ v*(b.a-a.a) + w*(c.a-a.a);
  pointOnSecond=a.b+ v*(b.b-a.b) + w*(c.b-a.b);
  return; //#0//#0
    }
};


struct epa_record
{
    collider* a;
    collider* b;
    glm::vec3 norm;
    polytope_prim polytope;
};

struct col_rec
{
    collider* a;
    collider* b;
    double depth;
    glm::vec3 norm;
    polytope_prim polytope;
    barycenter bcenter;

};

class Myphysicsdirector
{
public:
    static collider* player;
    static collider* getPlayer();
    static void setPlayer(collider* );
private:
    glm::vec3 gravity;
    glm::vec3 initial_search= glm::vec3(1,1,1);
    //std::vector<Node3D> test;
    bool dbg =false;
    bool physicson = false;
    bool not_step_mode = true;
    bool stepped = false;
    float physics_delta=0.0f;
    unsigned count=0;
    double ang_mom=0.0;
    float epsi=0.01f;
    double prev_time;
    double delta_time=0.0;
    epa_record temporary_epa_buffer;
    std::vector<col_rec> collision_recording;
    unsigned collision_recording_size=0;
    //make a collision record buffer space
        
    public:
        
        std::vector<Ref<collider>>axes;
        Myphysicsdirector();
        void start_sim();
        void _new_elements_added();
        void update_physics();
        void _process(float delta);
        void _physics_process(float delta);
        //TODO hey
        collider* _raycast(const glm::vec3 dir, const glm::vec3 pos, float max_ray);
        void set_gravity(const glm::vec3 p_gravity);
        glm::vec3 get_gravity() const;
        void set_initial_search(const glm::vec3 p_initial_search);
        glm::vec3 get_initial_search() const;
        void set_physicson(const bool p_physicson);
        bool get_physicson() const;
        void set_dbg(const bool p_dbg);
        bool get_dbg() const;
        void set_epsi(const float p_epsi);
        float get_epsi() const;
        bool gjk(collider& a, collider& b,bool debugmess=false);
        col_rec epa(collider& a, collider& b);
        bool gjk_next(simplex & pts, glm::vec3 & dir);
        bool gjk_line(simplex & pts, glm::vec3 & dir);
        bool gjk_tri(simplex & pts, glm::vec3 & dir);
        bool gjk_tetra(simplex & pts, glm::vec3 & dir);
        pointcombo support(collider& a, collider& b,const glm::vec3 & direction);
        // ON demand functions (these are designed to be called on demand and wont act up)
        
        bool on_demand_gjk(epa_record & col_pair);
        col_rec on_demand_epa(epa_record& col_pair);
        static void CreatePhysicsDirectorLuaBind(std::shared_ptr<sol::state> lua);
};


#endif