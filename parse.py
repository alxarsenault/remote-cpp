#!/usr/bin/python
import sys
import CppHeaderParser
import argparse

sys.path = ["../"] + sys.path

class ParamInfo:
	def __init__(self, method_param):
		self.ptype = method_param["type"];
		self.name = method_param["name"];

		#print("jkjkj " + self.ptype + " " + self.name)

	def get_str(self):
		return self.ptype + " " + self.name;

	def print(self):
		print(self.ptype + " " + self.name);

class MethodInfo:
	def __init__(self, method):
		self.name = method["name"];
		self.rtype = method["rtnType"];


		# method_params = 
		#print(self.name)


		#print("N PARAM %d" % len(method["parameters"]))

		self.params = []

		for p in method["parameters"]:
			self.params.append(ParamInfo(p));

		# print(self.get_param_list())


	def get_param_list(self):

		str_list = [];

		
		#print("N PARAM %d" % len(self.params))

		for p in self.params:
			str_list.append(p.get_str());


		str_list = list(', '.join(str_list))

		return ''.join(str_list)

	def get_param_names(self):

		str_list = [];
		for p in self.params:
			str_list.append(p.name);

		str_list = list(', '.join(str_list))

		# str = 
		# print(str);

		return ''.join(str_list)

	def print(self):
		print(self.rtype + " " + self.name);
		for p in self.params:
			p.print();

class Classinfo:
	def __init__(self, file_path, class_name, out_namespace = ""):

		try:
			cpp_header = CppHeaderParser.CppHeader(file_path)
		except CppHeaderParser.CppParseError as e:
			print(e)
			sys.exit(1)

		self.file_path = file_path;
		self.name = class_name;
		self.out_namespace = out_namespace;

		# self.include = [];
		# for incl in cpp_header.includes:
		# 	# print(" %s"%incl)
		# 	self.include.append("#include " + incl);
		
		interface_class = cpp_header.classes[class_name];
		public_method = interface_class["methods"]["public"];

		# print("NAME PSPACE : " + interface_class["namespace"])

		self.namespace = interface_class["namespace"]

		#print("N METHOD %d" % len(public_method))

		self.methods = [];

		for m in public_method:
			self.methods.append(MethodInfo(m));

	def create_method(self, method):
		ss = "virtual " + method.rtype + " " + method.name + "(";
		ss = ss + method.get_param_list() + ") {\n";

		if len(method.params) == 0:
			ss = ss + "\t\treturn call(RMTE_FUNC_INFO(" + self.name + "::" + method.name + "));\n\t}\n"
		else:
			ss = ss + "\t\treturn call(RMTE_FUNC_INFO(" + self.name + "::" + method.name + "), " + method.get_param_names() + ");\n\t}\n"
		
		#print(ss);

		return ss;

	def create_class(self, name):

		ss  = "";

		if len(self.out_namespace) != 0:
			ss = ss + "namespace " + self.out_namespace + " {\n";

		ss = ss + "class " + name + " : public ";

		if len(self.namespace) != 0:
			ss = ss + self.namespace + "::";

		ss = ss + self.name;
		
		ss = ss + ", public rmte::client {\n";
		ss = ss + "public :\n";

		for m in self.methods:
			ss = ss + "\t" + self.create_method(m) + "\n";

		ss = ss + "};"

		if len(self.out_namespace) != 0:
			ss = ss + "\n} // " + self.out_namespace;

		# print(ss);
		return ss;

	def create_header(self, name):
		header_str = "#pragma once\n\n";
		header_str = header_str + "#include <rmte/client.hpp>\n";
		header_str = header_str + '#include "' + self.file_path + '"\n\n';
		header_str = header_str + self.create_class(name) + "\n";

		#print(header_str)
		return header_str;

	def create_file(self, name, output_name):
		f = open(output_name, 'w')
		f.write(self.create_header(name));
		f.close();

	def print(self):
		for m in self.methods:
			m.print();


if __name__ == '__main__':
	parser = argparse.ArgumentParser(description = 'Gen')
	parser.add_argument('-f', dest='file_path', metavar='file_path', help='Header file path', required=True)
	parser.add_argument('-i', dest='interface_name', metavar='interface_name', help='Interface name', required=True)
	parser.add_argument('-c', dest='class_name', metavar='class_name', help='Class name', required=True)
	parser.add_argument('-o', dest='output_path', metavar='output_path', help='Output path', default="")
	parser.add_argument('-n', dest='namespace', metavar='namespace', help='namespace name', default="")
	args = parser.parse_args()

	cc = Classinfo(args.file_path, args.interface_name, args.namespace);
	print(cc.create_header(args.class_name));

	out_path = args.output_path

	if len(out_path) == 0:
		out_path = args.class_name;

	cc.create_file(args.class_name, out_path);



 