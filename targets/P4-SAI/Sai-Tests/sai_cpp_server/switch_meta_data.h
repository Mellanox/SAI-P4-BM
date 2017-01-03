#include  <sai.h>
#include <list>

class switch_metatdata{
// sai_object_id_t id;
// list of vlans
// 
};

class vlan_obj {
	sai_object_id_t sai_object_id;
	std::list<int> vlan_list;
	sai_vlan_id_t vlan_id = *vlan_list;	//vlan members 
//vid 

};

class port_obj {
	sai_object_id_t sai_object_id;
};

class bridge_obj {
	sai_object_id_t sai_object_id;

};

class bridge_port {
	sai_object_id_t sai_object_id;

};





 public:

  static const char* ascii_fingerprint; // = "9A73381FEFD6B67F432E717102246330";
  static const uint8_t binary_fingerprint[16]; // = {0x9A,0x73,0x38,0x1F,0xEF,0xD6,0xB6,0x7F,0x43,0x2E,0x71,0x71,0x02,0x24,0x63,0x30};

  Standard_bm_serialize_state_result(const Standard_bm_serialize_state_result&);
  Standard_bm_serialize_state_result& operator=(const Standard_bm_serialize_state_result&);
  Standard_bm_serialize_state_result() : success() {
  }

  virtual ~Standard_bm_serialize_state_result() throw();
  std::string success;

  _Standard_bm_serialize_state_result__isset __isset;

  void __set_success(const std::string& val);

  bool operator == (const Standard_bm_serialize_state_result & rhs) const
  {
    if (!(success == rhs.success))
      return false;
    return true;
  }
  bool operator != (const Standard_bm_serialize_state_result &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const Standard_bm_serialize_state_result & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

  friend std::ostream& operator<<(std::ostream& out, const Standard_bm_serialize_state_result& obj);
};