#include <algorithm>
#include <stdexcept>
#include <vector>
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "Model.hpp"
#include "Window.hpp"
#include "Callbacks.hpp"
#include "Debug.hpp"
#include "Shaders.hpp"
#include "SubDivMesh.hpp"
#include "SubDivMeshRenderer.hpp"

#define VERSION 20241025

// models and settings
std::vector<std::string> models_names = { "cubo", "icosahedron", "plano", "suzanne", "star" };
int current_model = 0;
bool fill = true, nodes = true, wireframe = true, smooth = false, 
	 reload_mesh = true, mesh_modified = false;

// extraa callbacks
void keyboardCallback(GLFWwindow* glfw_win, int key, int scancode, int action, int mods);

SubDivMesh mesh;
void subdivide(SubDivMesh &mesh);

int main() {
	
	// initialize window and setup callbacks
	Window window(win_width,win_height,"CG Demo");
	setCommonCallbacks(window);
	glfwSetKeyCallback(window, keyboardCallback);
	view_fov = 60.f;
	
	// setup OpenGL state and load shaders
	glEnable(GL_DEPTH_TEST); glDepthFunc(GL_LESS); 
	glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(0.8f,0.8f,0.9f,1.f);
	Shader shader_flat("shaders/flat"),
	       shader_smooth("shaders/smooth"),
		   shader_wireframe("shaders/wireframe");
	SubDivMeshRenderer renderer;
	
	// main loop
	Material material;
	material.ka = material.kd = glm::vec3{.8f,.4f,.4f};
	material.ks = glm::vec3{.5f,.5f,.5f};
	material.shininess = 50.f;
	
	FrameTimer timer;
	do {
		
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		
		if (reload_mesh) {
			mesh = SubDivMesh("models/"+models_names[current_model]+".dat");
			reload_mesh = false; mesh_modified = true;
		}
		if (mesh_modified) {
			renderer = makeRenderer(mesh,false);
			mesh_modified = false;
		}
		
		if (nodes) {
			shader_wireframe.use();
			setMatrixes(shader_wireframe);
			renderer.drawPoints(shader_wireframe);
		}
		
		if (wireframe) {
			shader_wireframe.use();
			setMatrixes(shader_wireframe);
			renderer.drawLines(shader_wireframe);
		}
		
		if (fill) {
			Shader &shader = smooth ? shader_smooth : shader_flat;
			shader.use();
			setMatrixes(shader);
			shader.setLight(glm::vec4{2.f,1.f,5.f,0.f}, glm::vec3{1.f,1.f,1.f}, 0.25f);
			shader.setMaterial(material);
			renderer.drawTriangles(shader);
		}
		
		// settings sub-window
		window.ImGuiDialog("CG Example",[&](){
			if (ImGui::Combo(".dat (O)", &current_model,models_names)) reload_mesh = true;
			ImGui::Checkbox("Fill (F)",&fill);
			ImGui::Checkbox("Wireframe (W)",&wireframe);
			ImGui::Checkbox("Nodes (N)",&nodes);
			ImGui::Checkbox("Smooth Shading (S)",&smooth);
			if (ImGui::Button("Subdivide (D)")) { subdivide(mesh); mesh_modified = true; }
			if (ImGui::Button("Reset (R)")) reload_mesh = true;
			ImGui::Text("Nodes: %i, Elements: %i",mesh.n.size(),mesh.e.size());
		});
		
		// finish frame
		window.finishFrame();
		
	} while( glfwGetKey(window,GLFW_KEY_ESCAPE)!=GLFW_PRESS && !glfwWindowShouldClose(window) );
}

void keyboardCallback(GLFWwindow* glfw_win, int key, int scancode, int action, int mods) {
	if (action==GLFW_PRESS) {
		switch (key) {
		case 'D': subdivide(mesh); mesh_modified = true; break;
		case 'F': fill = !fill; break;
		case 'N': nodes = !nodes; break;
		case 'W': wireframe = !wireframe; break;
		case 'S': smooth = !smooth; break;
		case 'R': reload_mesh=true; break;
		case 'O': case 'M': current_model = (current_model+1)%models_names.size(); reload_mesh = true; break;
		}
	}
}

// La struct Arista guarda los dos indices de nodos de una arista
// Siempre pone primero el menor indice, para facilitar la búsqueda en lista ordenada;
//    es para usar con el Mapa de más abajo, para asociar un nodo nuevo a una arista vieja
struct Arista {
	int n[2];
	Arista(int n1, int n2) {
		n[0]=n1; n[1]=n2;
		if (n[0]>n[1]) std::swap(n[0],n[1]);
	}
	Arista(Elemento &e, int i) { // i-esima arista de un elemento
		n[0]=e[i]; n[1]=e[i+1];
		if (n[0]>n[1]) std::swap(n[0],n[1]); // pierde el orden del elemento
	}
	const bool operator<(const Arista &a) const {
		return (n[0]<a.n[0]||(n[0]==a.n[0]&&n[1]<a.n[1]));
	}
};

// Mapa sirve para guardar una asociación entre una arista y un indice de nodo (que no es de la arista)
using Mapa = std::map<Arista,int>;

void subdivide(SubDivMesh &mesh) {
	
	/// @@@@@: Implementar Catmull-Clark... lineamientos:
	//  Los nodos originales estan en las posiciones 0 a #n-1 de m.n,
	int n_orig = mesh.n.size(); /// Guardo en n_orig la cantidad original de nodos de mesh.n
	//  Los elementos orignales estan en las posiciones 0 a #e-1 de m.e
	//  1) Por cada elemento, agregar el centroide (nuevos nodos: #n a #n+#e-1)
	for(int i=0;i<mesh.e.size();i++) { /// Para cada Elemento i
		glm::vec3 centroide_i_pos = glm::vec3(0.f);	/// Defino el vector posición del centroide del Elemento i como cero
		for(int j=0;j<mesh.e[i].nv;j++) { /// Para cada Nodo j del Elemento i
			int k = mesh.e[i].n[j];		  /// Almaceno en k el índice (en mesh.n) del Nodo j del Elemento i
			centroide_i_pos += mesh.n[k].p;	  /// Sumo la posición del Nodo k al vector posición del centroide del Elemento i
		}
		centroide_i_pos = centroide_i_pos/float(mesh.e[i].nv); 	/// Divido las posiciones sumadas por la cantidad de nodos del Elemento i para obtener la posición del centroide
		Nodo centroide_i = Nodo(centroide_i_pos);				/// Creo el Nodo centroide con la posición del centroide
		mesh.n.push_back(centroide_i);							/// Agrego el Nodo centroide a la lista de nodos n del mesh (mesh.n)
	}
	
	//  2) Por cada arista de cada cara, agregar un pto en el medio que es
	//      promedio de los vertices de la arista y los centroides de las caras 
	//      adyacentes. Aca hay que usar los elementos vecinos.
	//      En los bordes, cuando no hay vecinos, es simplemente el promedio de los 
	//      vertices de la arista
	//      Hay que evitar procesar dos veces la misma arista (como?)
	//      Mas adelante vamos a necesitar determinar cual punto agregamos en cada
	//      arista, y ya que no se pueden relacionar los indices con una formula simple
	//      se sugiere usar Mapa como estructura auxiliar
	
	Mapa mymap;
	
	for(int i=0;i<mesh.e.size();i++) { /// Para cada Elemento i
		for(int j=0;j<mesh.e[i].nv;j++) { /// Para cada Arista j del Elemento i
			glm::vec3 pmed_j_pos;	/// Defino el vector posición del punto medio de la Arista j
			Arista arista_j = Arista(mesh.e[i],j);	/// Almaceno en arista_j la j-ésima arista del Elemento i
			if(mymap.find(arista_j)==mymap.end()){
				mymap[arista_j]=mesh.n.size();
				int vecino_j = mesh.e[i].v[j];			/// Almaceno en vecino_j el vecino del Elemento i con el que comparte la arista_j
				if (vecino_j==-1){		/// Si el vecino_j es -1, entonces es frontera y se promedian solo los vértices de la arista_j
					pmed_j_pos = (mesh.n[arista_j.n[0]].p + mesh.n[arista_j.n[1]].p)/2.f;
				} else {				/// Sino, se promedian también los centroides
					glm::vec3 centroide_i_pos = mesh.n[n_orig+i].p;
					glm::vec3 centroide_j_pos = mesh.n[n_orig+vecino_j].p;
					pmed_j_pos = (centroide_i_pos + centroide_j_pos + mesh.n[arista_j.n[0]].p + mesh.n[arista_j.n[1]].p)/4.f;
				}
				Nodo pmed_j = Nodo(pmed_j_pos);	/// Creo el Nodo pmed_j con la posición del punto medio de la arista_j
				mesh.n.push_back(pmed_j);		/// Agrego el Nodo pmed_j a la lista de nodos n del mesh (mesh.n)
			}
		}
	}
	
	//  3) Armar los elementos nuevos
	//      Los quads se dividen en 4, (uno reemplaza al original, los otros 3 se agregan)
	//      Los triangulos se dividen en 3, (uno reemplaza al original, los otros 2 se agregan)
	//      Para encontrar los nodos de las aristas usar el mapa que armaron en el paso 2
	//      Ordenar los nodos de todos los elementos nuevos con un mismo criterio (por ej, 
	//      siempre poner primero al centroide del elemento), para simplificar el paso 4.
	
	int e_size_orig = mesh.e.size();	/// Almaceno la cantidad de elementos originales
	
	for(int i=0;i<e_size_orig;i++) { /// Para cada Elemento original i
		for(int j=1;j<mesh.e[i].nv;j++) { /// Para cada Nodo j del Elemento i
			int indice_centroide = n_orig + i;
			int indice_nodo_prev = mesh.e[i].n[j-1];
			int indice_nodo_j = mesh.e[i].n[j];
			int indice_nodo_next = mesh.e[i].n[j+1];
			
			Arista arista_prev = Arista(mesh.e[i],j);
			Arista arista_next = Arista(mesh.e[i],j-1);
			
			indice_nodo_prev = mymap[arista_prev];
			indice_nodo_next = mymap[arista_next];
			
			mesh.agregarElemento(indice_centroide,indice_nodo_next,indice_nodo_j,indice_nodo_prev);	/// Agrega
		}
		int j=0;
		int indice_centroide = n_orig + i;
		int indice_nodo_prev = mesh.e[i].n[j-1];
		int indice_nodo_j = mesh.e[i].n[j];
		int indice_nodo_next = mesh.e[i].n[j+1];
		
		Arista arista_prev = Arista(mesh.e[i],j);
		Arista arista_next = Arista(mesh.e[i],j-1);
		
		indice_nodo_prev = mymap[arista_prev];
		indice_nodo_next = mymap[arista_next];
		
		mesh.reemplazarElemento(i,indice_centroide,indice_nodo_next,indice_nodo_j,indice_nodo_prev);
	}
	
	mesh.makeVecinos();
	
	//  4) Calcular las nuevas posiciones de los nodos originales
	//      Para nodos interiores: (4r-f+(n-3)p)/n
	//         f=promedio de nodos interiores de las caras (los agregados en el paso 1)
	//         r=promedio de los pts medios de las aristas (los agregados en el paso 2)
	//         p=posicion del nodo original
	//         n=cantidad de elementos para ese nodo
	//      Para nodos del borde: (r+p)/2
	//         r=promedio de los dos pts medios de las aristas
	//         p=posicion del nodo original
	//      Ojo: en el paso 3 cambio toda la SubDivMesh, analizar donde quedan en los nuevos 
	//      elementos (¿de que tipo son?) los nodos de las caras y los de las aristas 
	//      que se agregaron antes.
	// tips:
	//   no es necesario cambiar ni agregar nada fuera de este método, (con Mapa como 
	//     estructura auxiliar alcanza)
	//   sugerencia: probar primero usando el cubo (es cerrado y solo tiene quads)
	//               despues usando la piramide (tambien cerrada, y solo triangulos)
	//               despues el ejemplo plano (para ver que pasa en los bordes)
	//               finalmente el mono (tiene mezcla y elementos sin vecinos)
	//   repaso de como usar un mapa:
	//     para asociar un indice (i) de nodo a una arista (n1-n2): elmapa[Arista(n1,n2)]=i;
	//     para saber si hay un indice asociado a una arista:  ¿elmapa.find(Arista(n1,n2))!=elmapa.end()?
	//     para recuperar el indice (en j) asociado a una arista: int j=elmapa[Arista(n1,n2)];
	for(int j=0;j<n_orig;j++) { /// Para cada Nodo original j
		if(mesh.n[j].es_frontera){
			/// Calcular n=cantidad de elementos para ese nodo
			int n = mesh.n[j].e.size();
			
			/// Calcular p=posicion del nodo original
			glm::vec3 p = mesh.n[j].p;
			
			/// Calcular r=promedio de los pts medios de las aristas (los agregados en el paso 2)
			glm::vec3 r = glm::vec3(0.f);
			for(int k=0;k<n;k++) { /// Para cada Elemento k que contiene al Nodo j
				/// Los indices a calcular son 1 y 3
				if (mesh.n[mesh.e[mesh.n[j].e[k]].n[1]].es_frontera){
					r += mesh.n[mesh.e[mesh.n[j].e[k]].n[1]].p;	/// Sumo la posición del punto medio
				}
				if (mesh.n[mesh.e[mesh.n[j].e[k]].n[3]].es_frontera){
					r += mesh.n[mesh.e[mesh.n[j].e[k]].n[3]].p;	/// Sumo la posición del punto medio
				}
				
			}
			r = r/2.f;	/// Promedio las posiciones
			
			mesh.n[j].p = (r+p)/2.f;
			
		}else{
			/// Calcular n=cantidad de elementos para ese nodo
			int n = mesh.n[j].e.size();
			
			/// Calcular p=posicion del nodo original
			glm::vec3 p = mesh.n[j].p;
			
			/// Calcular f=promedio de nodos interiores de las caras (los agregados en el paso 1)
			glm::vec3 f = glm::vec3(0.f);
			for(int k=0;k<n;k++) { /// Para cada Elemento k que contiene al Nodo j
				f = f + mesh.n[mesh.e[mesh.n[j].e[k]].n[0]].p ;	/// Sumo la posición del centroide del Elemento k
			}
			f = f/float(n);	/// Promedio las posiciones
			
			/// Calcular r=promedio de los pts medios de las aristas (los agregados en el paso 2)
			glm::vec3 r = glm::vec3(0.f);
			for(int k=0;k<n;k++) { /// Para cada Elemento k que contiene al Nodo j
				/// El indice a agarrar es siempre 1
				r += mesh.n[mesh.e[mesh.n[j].e[k]].n[1]].p;	/// Sumo la posición del punto medio
			}
			r = r/float(n);	/// Promedio las posiciones
			
			mesh.n[j].p = (4.f*r - f + (n-3.f)*p)/float(n);
		}
	}
	
	mesh.makeVecinos();
	
	
	// Esta llamada valida si la estructura de datos quedó consistente (si todos los
	// índices están dentro del rango válido, y si son correctas las relaciones
	// entre los .n de los elementos y los .e de los nodos). Mantener al final de
	// esta función para ver que la subdivisión implementada no rompa esos invariantes.
	mesh.verificarIntegridad();
}
