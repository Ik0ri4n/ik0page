<script lang="ts">
	import { ArrowSmDownIcon } from '@rgossiaux/svelte-heroicons/outline';
	import RevealableText from './RevealableText.svelte';

	export let name: string;
	export let category: string;
	export let author: string|undefined;
	export let solves: number;
	export let points: number;
	export let flag: string;
	export let downloads: string[];

	function get_name(path: string): string {
		path = path + '';
		return path.split('/').at(-1) || '';
	}

	let enabled = false;
</script>

<div
	class="rounded-md border border-gray-300 bg-gray-100 py-4 px-6 dark:border-gray-700 dark:bg-gray-800"
>
	<div
		class="not-prose mb-2 flex flex-wrap justify-between border-b border-gray-300 dark:border-gray-700"
	>
		<span class="mr-4">
			<h3 class="font-bold text-gray-900 dark:text-gray-50">{category} / {name}</h3>
			{#if author}
				<p class="mb-2">{author}</p>
			{/if}
		</span>
		<p class="ml-auto mr-4 mb-2 font-bold text-purple-700 dark:text-orange-500">
			{solves} solve{solves > 1 ? "s" : ""} / {points} points
		</p>
	</div>
	<slot />

	<div class="not-prose mb-4">
		<p class="text-sm font-medium mb-1">Flag:</p>
		<RevealableText description="flag" value={flag}/>
	</div>
	{#if downloads.length > 0}
		<div class="not-prose">
			<ul class="flex flex-row flex-wrap">
				{#each downloads as path}
					<li class="mr-4">
						<a
							href={path}
							class="inline-flex rounded-md border-2 border-green-700 py-1 px-2 text-sm font-bold text-green-700 hover:bg-green-700 hover:text-gray-50 focus:outline-none
                        focus:ring focus:ring-green-300 active:border-green-800 active:bg-green-800 active:text-gray-50 dark:border-green-500 dark:text-green-500 
                        dark:hover:bg-green-500 dark:hover:text-gray-900 dark:focus:ring-green-300 dark:active:border-green-600 dark:active:bg-green-600 dark:active:text-gray-900"
						>
							<ArrowSmDownIcon class="h-6 w-6" />
							<span>{get_name(path)}</span>
						</a>
					</li>
				{/each}
			</ul>
		</div>
	{/if}
</div>
