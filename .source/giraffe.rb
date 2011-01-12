require 'rbst'		# gem install maruku
require 'YAML'
require 'rubygems'
require 'coderay'

BASE_LINK = ""
LAYOUTS = {"basic" => "<html><head><title>{TITLE}</title></head><body>{BODY}</body></html>"}
SIDEBAR = {}

Dir.glob("layouts/*.html") do |f|
	fn = File.basename(f).split('.')[0]
	LAYOUTS[fn] = File.read(f)
end

Dir.glob("*.yaml").each do |f|
	fn = File.basename(f).split('.')[0]
	file = File.read(f)
	header, content = file.split('---',2)
	header.strip!
	header = YAML.load(header)
	title = header["title"] == nil ? "Page Name" : header["title"]
	if (header["sidebar"] == true)
		SIDEBAR[fn] = title
	end
end

# Generate SIDEBAR here.
sidebar_html = "<p class=\"sidebar\">"
SIDEBAR.each_pair do |name, title|
	sidebar_html << "<a href=\"#{BASE_LINK}#{name}.html\">#{title}</a> | "  
end
sidebar_html.chop!.chop!.chop!
sidebar_html << "</p>"



Dir.glob("*.yaml").each do |f|
	fn = File.basename(f).split('.')[0]
	file = File.read(f)
	header, content = file.split('---',2)
	header.strip!
	content.strip!
	header = YAML.load(header)
	html = header["layout"] == nil ? LAYOUTS["basic"] : LAYOUTS[header["layout"]]
	title = header["title"] == nil ? "Page Name" : header["title"]

	body  = RbST.new(content).to_html

	# Code
	body.gsub!(/(<!--.*-->)/m) do |v|
		CodeRay.scan(v.slice(4..-4).strip, :cpp).div(:css => :class)
	end

	# Internal links [[\link title]]
	body.gsub!(/(\[\[\/\w+\s\w+\]\])/) do |v|
		link, title = v.slice(3..-3).strip.split(" ", 2)
		"<a href=\"#{BASE_LINK}#{link}.html\">#{title}</a>"
	end

	# Images
	# => [[!test.png]] plain image
	# => [[!test.png L]] image float left
	# => [[!test.png R]] image float right
	# => [[!test.png C]] image center
	body.gsub!(/(\[\[!.*\]\])/) do |v|
		image, options = v.slice(3..-3).strip.split(" ", 2)
		#link, title = v.slice(3..-3).strip.split(" ", 2)
		#"<a href=\"#{BASE_LINK}#{link}\">#{title}</a>"
		ret = String.new
		if options == nil
			ret = "<img src=\"#{BASE_LINK}#{image}\" />"
		elsif options == "L"
			ret = "<div class=\"image-left\"><img src=\"#{BASE_LINK}#{image}\" /></div>"
		elsif options == "R"
			ret = "<div class=\"image-right\"><img src=\"#{BASE_LINK}#{image}\" /></div>"
		elsif options == "C"
			ret = "<div class=\"image-center\"><img src=\"#{BASE_LINK}#{image}\" /></div>"
		end
		
		ret
	end

	html.gsub!('{BODY}', body)
	html.gsub!('{SIDEBAR}', sidebar_html)
	html.gsub!('{TITLE}', title)
	File.open('../' + fn + '.html', 'w') {|f| f.write(html) }
end