require 'rbst'		# gem install maruku
require 'YAML'
require 'rubygems'
require 'coderay'
require 'digest/md5'

BASE_LINK = ""
LAYOUTS = {"basic" => "<html><head><title>{TITLE}</title></head><body>{BODY}</body></html>"}
SIDEBAR = {}

def makeLatex(string)
	string.gsub!("<!-- $$","")
	string.gsub!("-->","")
	string.strip!
	string.gsub!("||", '\\')

	# units
	string.gsub!(/(\{[^}]+\})/) do |v|
		"\\unit" + v
	end

	str = "\\documentclass[12pt]{article}\n\\pagestyle{empty}\n\\usepackage{mathtools}\n\\newcommand{\\unit}[1]{\\ensuremath{\\, \\mathrm{#1}}}\\begin{document}\n\\begin{align*}\n#{string}\n\\end{align*}\n\\end{document}\n"
	File.open('.temp/eq.tex', 'w') {|f| f.write(str) }	

    name = Digest::MD5.hexdigest(string).downcase
    name << ".png"

    system "latex -halt-on-error -output-directory .temp .temp/eq.tex"
    system "dvipng -T tight -bg Transparent -o ../eq/#{name} .temp/eq.dvi"

    "<div class=\"equation-block\"><img src=\"eq/#{name}\" alt=\"#{string}\" /></div>"
end

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
sidebar_html = String.new
SIDEBAR.each_pair do |name, title|
	sidebar_html << "<a href=\"#{BASE_LINK}#{name}.html\">#{title}</a> | "  
end
sidebar_html.chop!.chop!.chop!



Dir.glob("*.yaml").each do |f|
	fn = File.basename(f).split('.')[0]
	puts fn
	file = File.read(f)
	header, content = file.split('---',2)
	header.strip!
	content.strip!
	header = YAML.load(header)
	html = String.new
	if header["layout"] == nil
		html << LAYOUTS["basic"]
	else
		html << LAYOUTS[header["layout"]]
	end
	
	title = header["title"] == nil ? "Page Name" : header["title"]

	body  = RbST.new(content).to_html

	# hr
	body.gsub!("<p>---</p>", "<hr />")

	# Latex
	body.gsub!(/(<!--\s\$\$[^>]*-->)/m) do |v|
		makeLatex(v)
	end

	# Code
	body.gsub!(/(<!--?[^>]*-->)/m) do |v|
		t = CodeRay.scan(v.slice(4..-4).strip, :cpp).div(:css => :class)
		t.strip!
		t.gsub!("<div class=\"CodeRay\">","")
		t.chop!.chop!.chop!.chop!.chop!.chop!.strip! # remove </div>
		t.gsub!("class=\"code\"", "class=\"code-block\"")
		t
	end

	# Inline code
	body.gsub!(/(&#64;[\w\s\d\*-_><\(\)\[\]\{\}]+&#64;)/) do |v|
		"<code class=\"code-inline\">" + v.slice(5..-6) + "</code>"
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

	# Heading with anchors [[$name]]
	body.gsub!(/(<p>\=\=\=\=.+<\/p>)/) do |v|
		l, t = v.slice(7..-5).strip.split(" ", 2)
		puts l
		puts t
		"<h2><a name=\"#{l}\"></a>#{t}</h2>"
	end

	# Internal links [[link title]]
	body.gsub!(/(\[\[?[^\]]*\]\])/) do |v|
		puts v
		l, t = v.slice(2..-3).strip.split(" ", 2)
		if (l.index('.'))
			l.gsub!(".",".html#")
		else
			l << ".html"
		end
		"<a href=\"#{BASE_LINK}#{l}\">#{t}</a>"
	end

	html.gsub!('{BODY}', body)
	html.gsub!('{SIDEBAR}', sidebar_html)
	html.gsub!('{TITLE}', title)
	File.open('../' + fn + '.html', 'w') {|f| f.write(html) }
end