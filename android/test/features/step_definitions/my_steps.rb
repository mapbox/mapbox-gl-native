
Then /^I await for ([\d\.]+) second(?:s)?$/ do |num_seconds|
num_seconds = num_seconds.to_f
sleep num_seconds

end
